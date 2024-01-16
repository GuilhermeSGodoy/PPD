
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

void save_matrix(double *matrix, int size, int rank) {
    char file_name[30];
    sprintf(file_name, "result_matrix_rank_%d.txt", rank);

    FILE *file;
    file = fopen(file_name, "w");

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%5.2f ", matrix[i * size + j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mpirun -np <num_procs> ./mmul_mpi N\n");
        printf("N: Matrix size on each axis\n");
        exit(-1);
    }

    int size, rank, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        printf("Number of processes: %d\n", num_procs);
    }

    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    int total_elements = size * size;

    double *a = (double *)malloc(total_elements * sizeof(double));
    double *b = (double *)malloc(total_elements * sizeof(double));

    if (rank == 0) {
        for (int i = 0; i < total_elements; i++) {
            a[i] = i + 1;
            b[i] = i + 1;
        }
    }

    double *c = (double *)malloc(total_elements * sizeof(double));

    MPI_Bcast(a, total_elements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, total_elements, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&time_start, NULL);

    int chunk_size = size / num_procs;
    
    // Alocação de uma nova matriz referente aos resultados locais. Isso se fez necessário
    // devido a um erro de "Buffers must not be aliased". Após algumas tentativas, essa acabou
    // sendo a solução que teve um resultado positivo.
    double *local_c = (double *)malloc(chunk_size * size * sizeof(double));

    for (int i = rank * chunk_size; i < (rank + 1) * chunk_size; i++) {
        for (int j = 0; j < size; j++) {
            local_c[(i - rank * chunk_size) * size + j] = 0;

            for (int k = 0; k < size; k++) {
                local_c[(i - rank * chunk_size) * size + j] += a[i * size + k] * b[k * size + j];
            }
        }
    }

    MPI_Gather(local_c, chunk_size * size, MPI_DOUBLE, c, chunk_size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    gettimeofday(&time_end, NULL);

    save_matrix(local_c, chunk_size, rank);

    if (rank == 0) {
        save_matrix(c, size, rank);
        double exec_time = (double)(time_end.tv_sec - time_start.tv_sec) +
                           (double)(time_end.tv_usec - time_start.tv_usec) / 1000000.0;

        printf("Matrices multiplied in %lf seconds\n", exec_time);
    }

    free(a);
    free(b);
    free(c);
    free(local_c);

    MPI_Finalize();

    return 0;
}
