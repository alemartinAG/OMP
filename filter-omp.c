#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <omp.h>

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/* nombre del archivo a leer */
#define FILE_NAME "OR_ABI-L2-CMIPF-M6C02_G16_s20191011800206_e20191011809514_c20191011809591.nc"

/* Lectura de una matriz de 21696 x 21696 */
#define NX 21696
#define NY 21696
#define WS 3
#define NAN -1

#define TNUM 4

float data_in[NX][NY];
float data_out[NX][NY];

int w[WS][WS] = {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
            };

void convolucion(int, int);

int main()
{
    int ncid, varid;
    int x, y, retval;

    if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid)))
        ERR(retval);
    
    /* Obtenemos elvarID de la variable CMI. */
    if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
        ERR(retval);

    /* Leemos la matriz. */
    if ((retval = nc_get_var_float(ncid, varid, &data_in[0][0])))
        ERR(retval);

    printf("PROCESANDO con %d threads...\n", TNUM);
    

    omp_set_num_threads(TNUM);
    double start_time = omp_get_wtime();
    #pragma omp parallel
    {
    	int threadNum = omp_get_thread_num();
    	//convolute(threadNum);
    	#pragma omp for collapse(2)
    	for (int i=0; i<NX; ++i){
    		for (int j=0; j<NY; ++j){
    			convolucion(i, j);
    		}
    	}

    	#pragma omp barrier

    	printf("GUARDANDO ARCHIVO - %d\n", threadNum);

    	char * filename;
        filename = (char *) calloc(sizeof(char), 30);
    	sprintf(filename, "parte%02d.pgm", threadNum);

    	int beg = (NX/TNUM) * threadNum;
    	int end = (NX/TNUM) * (threadNum+1);
    	int height = (NX/TNUM);

    	/*FILE *fd;
    	fd = fopen(filename, "wb");

    	float *ptr;
    	ptr = &data_out[0][0];
    	ptr += beg;

    	fwrite(ptr, sizeof(float), tam, fd);

    	fclose(fd);*/

    	FILE* pgmimg; 
    	pgmimg = fopen(filename, "wb");

        free(filename);
  
    	// Writing Magic Number to the File 
    	fprintf(pgmimg, "P2\n");  
  
    	// Writing Width and Height 
    	fprintf(pgmimg, "%d %d\n", NY, height);
  
    	// Writing the maximum gray value 
    	fprintf(pgmimg, "255\n");
    	
    	for (int i=beg; i<end; i++) { 
        	for (int j=0; j<NY; j++) {
           	 	// Writing the gray values in the 2D array to the file 
            	fprintf(pgmimg, "%d ", (int) data_out[i][j]);
        	}
        	fprintf(pgmimg, "\n");
    	} 

    	fclose(pgmimg);

	}

    double time = omp_get_wtime() - start_time;

    printf("FINALIZADO EN %f SEGUNDOS\n", time);

    /* Se cierra el archivo y liberan los recursos*/
    if ((retval = nc_close(ncid))){
        ERR(retval);
    }

    /*printf("\nGENERANDO IMAGEN...\n");

    FILE *fp;
    fp = fopen("imagen2.png", "wb");

    fwrite(data_in, sizeof(float), sizeof(data_in), fp);
    fclose(fp);*/

    return 0;
}

void convolucion(int i, int j){
	
	data_out[i][j] = 0;

	for(int k=0; k<WS; k++){

        if(data_in[i][j] == NAN){
            data_out[i][j] = NAN;
            break;
        }

        for(int l=0; l<WS; l++){
            data_out[i][j] += w[k][l]*data_in[i-k][j-l];
        }
    }
}
