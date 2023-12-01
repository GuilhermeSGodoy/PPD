#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define ITER_MAX 3000          // number of maximum iterations
#define CONV_THRESHOLD 1.0e-5f // threshold of convergence

// matrix to be solved
double **grid;

// auxiliary matrix
double **new_grid;

// size of each side of the grid
int size;

// number of threads
int num_threads; // default to 1 thread

// thread structure to pass arguments
struct ThreadArgs {
    int start;
    int end;
    int thread_id;
    double local_err; // local error for each thread
};

// return the maximum value
double max(double a, double b) {
    if (a > b)
        return a;
    return b;
}

// return the absolute value of a number
double absolute(double num) {
    if (num < 0)
        return -1.0 * num;
    return num;
}

// Variável global para armazenar o erro
double err = 1.0;

// allocate memory for the grid
void allocate_memory() {
    grid = (double **)malloc(size * sizeof(double *));
    new_grid = (double **)malloc(size * sizeof(double *));

    for (int i = 0; i < size; i++) {
        grid[i] = (double *)malloc(size * sizeof(double));
        new_grid[i] = (double *)malloc(size * sizeof(double));
    }
}

// initialize the grid
void initialize_grid() {
    // seed for random generator
    srand(10);

    int linf = size / 2;
    int lsup = linf + size / 10;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // initialize heat region in the center of the grid
            if (i >= linf && i < lsup && j >= linf && j < lsup)
                grid[i][j] = 100;
            else
                grid[i][j] = 0;
            new_grid[i][j] = 0.0;
        }
    }
}

// save the grid in a file
void save_grid() {
    char file_name[30];
    sprintf(file_name, "grid_laplace.txt");

    // save the result
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

// Jacobi iteration for a range of rows
void *jacobi_iteration(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;

    int start = args->start;
    int end = args->end;
    int thread_id = args->thread_id;

    for (int i = start; i <= end; i++) {
        for (int j = 1; j < size - 1; j++) {
            new_grid[i][j] = 0.25 * (grid[i][j + 1] + grid[i][j - 1] +
                                     grid[i - 1][j] + grid[i + 1][j]);
            args->local_err = max(args->local_err, absolute(new_grid[i][j] - grid[i][j]));
        }
    }

    pthread_exit(NULL);
}

// copy the next values into the working array for the next iteration
void copy_values() {
    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: ./laplace_seq N NUM_THREADS\n");
        printf("N: The size of each side of the domain (grid)\n");
        printf("NUM_THREADS: The number of threads to use\n");
        exit(-1);
    }

    // variables to measure execution time
    struct timeval time_start;
    struct timeval time_end;

    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    // allocate memory to the grid (matrix)
    allocate_memory();

    // set grid initial conditions
    initialize_grid();

    int iter = 0;

    printf("Jacobi relaxation calculation: %d x %d grid with %d threads\n", size, size, num_threads);

    // get the start time
    gettimeofday(&time_start, NULL);

    // Jacobi iteration
    // This loop will end if either the maximum change reaches below a set threshold (convergence)
    // or a fixed number of maximum iterations have completed
    do {
        err = 0.0;

        pthread_t threads[num_threads];
        struct ThreadArgs args[num_threads];

        // Divide the work among threads
        int rows_per_thread = size / num_threads;
        for (int t = 0; t < num_threads; t++) {
            args[t].start = t * rows_per_thread + 1;
            args[t].end = (t == num_threads - 1) ? size - 2 : (t + 1) * rows_per_thread;
            args[t].thread_id = t;
            args[t].local_err = 0.0;

            pthread_create(&threads[t], NULL, jacobi_iteration, (void *)&args[t]);
        }

        // Wait for threads to finish
        for (int t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
            err = max(err, args[t].local_err);
        }

        // copy the next values into the working array for the next iteration
        copy_values();

        if (iter % 100 == 0)
            printf("Error of %0.10lf at iteration %d\n", err, iter);

        iter++;

        if (iter >= ITER_MAX) {
            printf("Maximum number of iterations reached.\n");
            break;
        }
    } while (err > CONV_THRESHOLD);

    // get the end time
    gettimeofday(&time_end, NULL);

    double exec_time = (double)(time_end.tv_sec - time_start.tv_sec) +
                       (double)(time_end.tv_usec - time_start.tv_usec) / 1000000.0;

    // save the final grid in a file
    save_grid();

    printf("\nKernel executed in %lf seconds with %d iterations and error of %0.10lf and with %d threads\n", exec_time, iter, err, num_threads);

    return 0;
}
