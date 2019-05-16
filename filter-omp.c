#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <omp.h>
#include <string.h>

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/* nombre del archivo a leer */
#define FILE_NAME "OR_ABI-L2-CMIPF-M6C02_G16_s20191011800206_e20191011809514_c20191011809591.nc"

/* Lectura de una matriz de 21696 x 21696 */
#define NX 21696
#define NY 21696
#define WS 3    // tamaño de matriz de filtro
#define NAN -1  // Not A Number

#define TNUM 4  // numero de threads
#define COMPR 2 // razon de compresion de imagen

#define chunk 2048 // tamaño de cache

// matriz filtro
int w[WS][WS] = {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
            };

void createNetCDF(short *);

int main()
{

    /* Abrimos archivo NetCDF y obtenemos matriz la matriz */

    short *data_in;
    data_in = (short *) calloc(NY*NX, sizeof(short)); //TODO: free

    int ncid, varid;
    int x, y, retval;

    if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid)))
        ERR(retval);
    
    /* Obtenemos elvarID de la variable CMI. */
    if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
        ERR(retval);

    /* Leemos la matriz. */
    //if ((retval = nc_get_var_float(ncid, varid, &data_in[0][0])))
    if ((retval = nc_get_var_short(ncid, varid, &data_in[0])))
        ERR(retval);

    /* Se cierra el archivo y liberan los recursos*/
    if ((retval = nc_close(ncid))){
        ERR(retval);
    }

    /* Creo matriz resultante */

    short *data_out;
    data_out = (short *) calloc(NX*NY, sizeof(short));


    printf("\nPROCESANDO CON %d THREADS...\n\n", TNUM);

    omp_set_num_threads(TNUM);
    double start_time = omp_get_wtime();
    #pragma omp parallel
    {

        int threadNum = omp_get_thread_num();

    	#pragma omp for collapse(2) //schedule(static, chunk)
    	for (int i=0; i<NX; ++i){
    		for (int j=0; j<NY; ++j){

                data_out[i*NX + j] = 0;

                for(int k=0; k<WS; k++){
                    if(data_in[i*NX+j] == NAN){
                        data_out[i*NX+ j] += 0;
                        //break;
                    }
                    for(int l=0; l<WS; l++){
                        data_out[i*NX+j] += ((short)w[k][l]) * data_in[(i-k-1)*NX+(j-l-1)];
                    }
                }

                /* Valores negativos los pongo a 0 */
                if(data_out[i*NX+j] < 0){
                    data_out[i*NX+j] = NAN;
                }
    		}
    	}
	}

    double time = omp_get_wtime() - start_time;
    printf("Convolucion finalizada @%f s\n", time);

    omp_set_num_threads(TNUM);
    #pragma omp parallel
    {

        int threadNum = omp_get_thread_num();

        FILE *head;
        head = fopen("Imagen/parte00", "wb");
        
        char * header;
        header = (char *) calloc(sizeof(char), 64);
        sprintf(header, "P2\n%d %d\n13000\n", NY/COMPR, NX/COMPR);
        fwrite(header, sizeof(char), strlen(header), head);
        free(header);

        fclose(head);

        char * filename;
        filename = (char *) calloc(sizeof(char), 30);
        sprintf(filename, "Imagen/parte%02d", threadNum+1);

        int beg = (NX/TNUM) * threadNum;
        int end = (NX/TNUM) * (threadNum+1);
        int height = (NX/TNUM);

        FILE* fragment; 
        fragment = fopen(filename, "wb");
        free(filename);
        
        for (int i=beg; i<end; i++) { 
            for (int j=0; j<NY; j++) {
                // Writing the gray values in the 2D array to the file
                fprintf(fragment, "%d ", (int) data_out[i*NX + j]);
                //fprintf(fragment, "%d ", (int) data_in[i + j*NY]);
                for(int l=1; l<COMPR; l++){
                    j++;
                }
            }
            fprintf(fragment, "\n");
            
            for(int l=1; l<COMPR; l++){
                i++;
            }
        } 

        fclose(fragment);

    }

    system("Scripts/generar_imagen.sh");

    time = omp_get_wtime() - start_time;
    printf("Creacion de imagen finalizada @%f s\n", time);

    createNetCDF(&data_out[0]);

    time = omp_get_wtime() - start_time;
    printf("creacion de .nc @%f s\n", time);
    
    // Libero matrices
    free(data_in);
    free(data_out);

    return 0;
}

void createNetCDF(short * data_out){

    char filename[32] = {"filtered.nc"};
    int ncid, x_dimid, y_dimid, varid, retval;
    int NDIMS = 2;
    int dimids[NDIMS];

    // Creamos archivo, si ya existe no se sobreescribe
    if ((retval = nc_create(filename, NC_NOCLOBBER, &ncid)))
        ERR(retval);

    /* Definimos dimensiones */
    if ((retval = nc_def_dim(ncid, "x", NX, &x_dimid)))
      ERR(retval);
    if ((retval = nc_def_dim(ncid, "y", NY, &y_dimid)))
      ERR(retval);

    dimids[0] = x_dimid;
    dimids[1] = y_dimid;

    // Definimos la variable
    if ((retval = nc_def_var(ncid, "CMI", NC_SHORT, NDIMS, 
                dimids, &varid)))
    ERR(retval);

    /* End define mode. This tells netCDF we are done defining
    * metadata. */
    if ((retval = nc_enddef(ncid)))
      ERR(retval);

    /* Write the pretend data to the file. Although netCDF supports
    * reading and writing subsets of data, in this case we write all
    * the data in one operation. */
    if ((retval = nc_put_var_short(ncid, varid, &data_out[0])))
      ERR(retval);

    /* Close the file. This frees up any internal netCDF resources
    * associated with the file, and flushes any buffers. */
    if ((retval = nc_close(ncid)))
      ERR(retval);

}