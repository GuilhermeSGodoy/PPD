CC=gcc

all: clean openmp

openmp:
	$(CC) laplace_omp.c -o laplace_omp -fopenmp

clean:
	rm -f laplace_omp