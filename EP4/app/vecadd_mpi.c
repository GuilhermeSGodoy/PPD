#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"

#define TOL 0.0000001

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: mpirun -np <num_processes> ./vecadd_mpi N\n");
            printf("N: Size of the vectors\n");
        }
        MPI_Finalize();
        exit(-1);
    }

    struct timeval time_start;
    struct timeval time_end;

    gettimeofday(&time_start, NULL);

    int size = atoi(argv[1]);
    int err = 0;
    int chunk_size = size / num_processes;

    float *local_a = (float *)malloc(chunk_size * sizeof(float));
    float *local_b = (float *)malloc(chunk_size * sizeof(float));
    float *local_c = (float *)malloc(chunk_size * sizeof(float));
    float *local_res = (float *)malloc(chunk_size * sizeof(float));

    for (int i = 0; i < chunk_size; i++) {
        int global_index = rank * chunk_size + i;
        local_a[i] = (float)global_index;
        local_b[i] = 2.0 * (float)global_index;
        local_c[i] = 0.0;
        local_res[i] = global_index + 2 * global_index;
    }

    for (int i = 0; i < chunk_size; i++) {
        local_c[i] = local_a[i] + local_b[i];
    }

    float *c = NULL;
    if (rank == 0) {
        c = (float *)malloc(size * sizeof(float));
    }

    MPI_Gather(local_c, chunk_size, MPI_FLOAT, c, chunk_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            float val = c[i] - local_res[i];
            val = val * val;

            if (val > TOL)
                err++;
        }

        gettimeofday(&time_end, NULL);

        double exec_time = (double)(time_end.tv_sec - time_start.tv_sec) +
                           (double)(time_end.tv_usec - time_start.tv_usec) / 1000000.0;

        printf("vectors added with %d errors in %lf seconds with %d processes\n", err, exec_time, num_processes);

        free(c);
    }

    free(local_a);
    free(local_b);
    free(local_c);
    free(local_res);

    MPI_Finalize();

    return 0;
}
