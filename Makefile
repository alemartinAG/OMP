CC=gcc
CFLAGS=-Werror​ -Wall -pedantic

all:
	$(CC) readNetcdf.c -I/home/ale/Netcdf/SO2/hpc/libs/netcdf/include -L/home/ale/Netcdf/SO2/hpc/libs/netcdf/lib -lnetcdf -lm -o read
	$(CC) filter-omp.c -I/home/ale/Netcdf/SO2/hpc/libs/netcdf/include -L/home/ale/Netcdf/SO2/hpc/libs/netcdf/lib -lnetcdf -lm -o filter-omp -fopenmp
omp:
	$(CC) filter-omp.c -I/home/ale/Netcdf/SO2/hpc/libs/netcdf/include -L/home/ale/Netcdf/SO2/hpc/libs/netcdf/lib -lnetcdf -lm -o filter-omp -fopenmp
clean:
	#rm read
	rm filter-omp
	rm -r Imagen
	mkdir Imagen