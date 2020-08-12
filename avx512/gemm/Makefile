all: gemmbench dgemmbench

CFLAGS += -march=native -g -O3 

gemmbench: gemmbench.o mygemm.o  Makefile
	gcc -O3 -Wall -W -march=native gemmbench.o mygemm.o -o gemmbench   -lopenblas -lpthread -lgfortran -fopenmp

dgemmbench: dgemmbench.o mydgemm.o  Makefile
	gcc -O3 -Wall -W -march=native dgemmbench.o mydgemm.o -o dgemmbench   -lopenblas -lpthread -lgfortran -fopenmp

	
clean:
	rm -f gemmbench *~ reference.dat* *.o
