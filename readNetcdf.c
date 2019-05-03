#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <time.h>

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

float data_in[NX][NY];
float data_out[NX][NY];

int w[WS][WS] = {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
            };

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

    printf("PROCESANDO...\n");

    //Reloj
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();


    for(int i=2; i<NX; i++){
        for(int j=2; j<NY; j++){
            
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
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("FINALIZADO EN %f SEGUNDOS\n", cpu_time_used);

    /* Se cierra el archivo y liberan los recursos*/
    if ((retval = nc_close(ncid)))
        ERR(retval);

    /*printf("\nGENERANDO IMAGEN...\n");

    FILE *fp;
    fp = fopen("imagen2.png", "wb");

    fwrite(data_in, sizeof(float), sizeof(data_in), fp);
    fclose(fp);*/

    return 0;
}
