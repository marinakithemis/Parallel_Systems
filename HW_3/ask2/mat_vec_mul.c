#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void print_vector(float *vector, int size) {
    for (int i = 0; i < size; i++) {
        printf("%6.2f ", vector[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, comm_sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n, col_count;
    float *matrix = NULL;
    float *vec = NULL;

    int exit_flag = 0;

    if (rank == 0) {
        if (argc > 1) {
            if (argc != 2) {
                fprintf(stderr, "Usage: ./mat_vec_mul <grid_size>\n");
                exit_flag = 1;
            } 
            else {
                char *endptr;
                n = strtoul(argv[1], &endptr, 0);
                if (*endptr != '\0') {
                    fprintf(stderr, "Invalid grid size\n");
                    exit_flag = 1;
                }
                else if (n <= 0 || n % comm_sz) {
                    fprintf(stderr, "Error: n must be a positive integer divisible by comm_sz\n");
                    exit_flag = 1;
                }
            } 
        }
        else {
            // Default value
            n = 8;
        }
    }

    MPI_Bcast(&exit_flag, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (exit_flag) {
        MPI_Finalize();
        return 1;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        col_count = n / comm_sz;
        // Initialize the matrix in rank 0
        matrix = malloc(n * n * sizeof(float));
        vec = malloc(n * sizeof(float));
        for (int i = 0; i < n; i++) {
            vec[i] = i; 
            for (int j = 0; j < n; j++) {
                matrix[i * n + j] = i + j; // Fill with dummy data
            }
        }
    }
    // Broadcast how many cols each proc will handle
    MPI_Bcast(&col_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    float *result = calloc(n , sizeof(float)); // this vector will hold tmp sums, init to 0
    float *col_buf = malloc(col_count * n * sizeof(float)); //array of columns
    float* elem_buf = malloc(col_count * sizeof(float));    //array of elements from vector

    double start_s, finish_s, elapsed_s, s_elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    start_s = MPI_Wtime();
    //Scatter source https://engineering.purdue.edu/~smidkiff/KKU/files/MPI2.pdf
    MPI_Datatype col, coltype;
    MPI_Type_vector(n, 1, n, MPI_FLOAT,&col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(float), &coltype);
    MPI_Type_commit(&coltype);

    // Scatter the columns and elements
    MPI_Scatter(matrix, col_count, coltype, col_buf, col_count * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec, col_count, MPI_FLOAT, elem_buf, col_count, MPI_FLOAT, 0, MPI_COMM_WORLD);
    finish_s = MPI_Wtime();
    elapsed_s = finish_s - start_s;
    MPI_Reduce(&elapsed_s, &s_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    double local_start, local_finish, elapsed, local_elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();

    // Compute the partial sum
    for (int i = 0; i < col_count; i++) {
        for (int j = 0; j < n; j++) {
            result[j] += col_buf[i * n + j] * elem_buf[i];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    double start_r, finish_r, elapsed_r, r_elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    start_r = MPI_Wtime();
    // reduction for sum of all partial results back to initial vector
    MPI_Reduce(result, vec, n, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    finish_r = MPI_Wtime();
    elapsed_r = finish_r - start_r;
    MPI_Reduce(&elapsed_r, &r_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        #ifdef OUT
            printf("Result vector:\n");
            print_vector(vec, n);
        #endif
        printf("Scatter: %f\n", s_elapsed);
        printf("Algorithm: %f\n", elapsed);
        printf("Reduce: %f\n", elapsed_r);
        free(vec);
        free(matrix);
    }
    
    // clean up
    free(col_buf);
    free(elem_buf);
    free(result);
    MPI_Type_free(&coltype);
    MPI_Type_free(&col);
    MPI_Finalize();
    return 0;
}
