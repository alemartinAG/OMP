CC=gcc
CFLAGS=-Werror​ -Wall -pedantic

all:
	$(CC) filter-omp.c -I/home/ale/Netcdf/SO2/hpc/libs/netcdf/include -L/home/ale/Netcdf/SO2/hpc/libs/netcdf/lib -lnetcdf -lm -o filter-omp.out -fopenmp
mono:
	$(CC) readNetcdf.c -I/home/ale/Netcdf/SO2/hpc/libs/netcdf/include -L/home/ale/Netcdf/SO2/hpc/libs/netcdf/lib -lnetcdf -lm -o filter-mono.out
cppcheck:
	cppcheck --enable=all  --suppress=missingIncludeSystem *.c 2>cpp_check.txt
clean:
	rm *.out
	rm -r Imagen
	mkdir Imagen
	rm filtered.nc