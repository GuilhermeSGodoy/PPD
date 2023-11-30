#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define ITER_MAX 3000
#define CONV_THRESHOLD 1.0e-5f
#define NUM_THREADS 4

double **grid;
double **new_grid;
int size;

pthread_barrier_t barrier;
pthread_mutex_t err_mutex;

double max(double a, double b) {
    return (a > b) ? a : b;
}

double absolute(double num) {
    return (num < 0) ? -1.0 * num : num;
}

void allocate_memory() {
    grid = (double **)malloc(size * sizeof(double *));
    new_grid = (double **)malloc(size * sizeof(double *));

    for (int i = 0; i < size; i++) {
        grid[i] = (double *)malloc(size * sizeof(double));
        new_grid[i] = (double *)malloc(size * sizeof(double));
    }
}

void initialize_grid() {
    srand(10);

    int linf = size / 2;
    int lsup = linf + size / 10;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i >= linf && i < lsup && j >= linf && j < lsup)
                grid[i][j] = 100;
            else
                grid[i][j] = 0;
            new_grid[i][j] = 0.0;
        }
    }
}

void save_grid() {
    char file_name[30];
    sprintf(file_name, "grid_laplace.txt");

    FILE *file;
    file = fopen(file_name, "w");

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%lf ", grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void *jacobi_iteration(void *arg) {
    int thread_id = *(int *)arg;
    int start_row = thread_id * (size / NUM_THREADS);
    int end_row = (thread_id + 1) * (size / NUM_THREADS);

    for (int iter = 0; iter <= ITER_MAX; iter++) {
        double local_err = 0.0;

        for (int i = start_row + 1; i < end_row - 1; i++) {
            for (int j = 1; j < size - 1; j++) {
                new_grid[i][j] = 0.25 * (grid[i][j + 1] + grid[i][j - 1] +
                                         grid[i - 1][j] + grid[i + 1][j]);

                local_err = max(local_err, absolute(new_grid[i][j] - grid[i][j]));
            }
        }

        pthread_barrier_wait(&barrier);

        if (thread_id == 0) {
            pthread_mutex_lock(&err_mutex);
            double global_err = 0.0;

            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    grid[i][j] = new_grid[i][j];
                    global_err = max(global_err, absolute(new_grid[i][j] - grid[i][j]));
                }
            }

            pthread_mutex_unlock(&err_mutex);

            if (iter % 100 == 0) {
                printf("Error of %0.10lf at iteration %d\n", global_err, iter);
            }

            if (global_err < CONV_THRESHOLD) {
                iter = ITER_MAX; // Interrompe todas as threads
            }
        }

        pthread_barrier_wait(&barrier);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./laplace_seq N\n");
        printf("N: The size of each side of the domain (grid)\n");
        exit(-1);
    }

    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    pthread_mutex_init(&err_mutex, NULL);

    allocate_memory();
    initialize_grid();

    printf("Jacobi relaxation calculation: %d x %d grid\n", size, size);

    gettimeofday(&time_start, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, jacobi_iteration, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&time_end, NULL);

    double exec_time = (double)(time_end.tv_sec - time_start.tv_sec) +
                       (double)(time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    save_grid();

    printf("\nKernel executed in %lf seconds with error below %0.10lf\n", exec_time, CONV_THRESHOLD);

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&err_mutex);

    return 0;
}
