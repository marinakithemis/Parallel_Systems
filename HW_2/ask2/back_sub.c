#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <omp.h>
#include <assert.h>
#include "../utils.h"
#include "../timer.h"

void init_serial(float ***A, float **b, float **x, int size){
    *A = malloc(size * sizeof(float *)); MEMCHECK(A);
    float **matrix = *A;
    matrix[0] = calloc(size*size, sizeof(float)); MEMCHECK(matrix[0]);
    *b = malloc(size * sizeof(float)); MEMCHECK(b);
    *x = malloc(size * sizeof(float)); MEMCHECK(x);

    for (int i = 0; i < size; i++) {
        float tmp = rand() % 101;
        (*b)[i] =  tmp == 50 ? 1 : tmp-50;

        matrix[i] = matrix[0] + i*size;
        for (int j = i; j < size; j++) {
            float tmp = rand() % 101;
            matrix[i][j] = tmp == 50 ? 1 : tmp-50; // values from -50 to 50, excluding 0
        }
    }
}

void back_sub_row_serial(float **A, float *b, float *x, int n){
    for(int row=n-1; row>=0; row--){
        x[row] = b[row];
        for(int col = row+1; col<n; col++){
            x[row] -= A[row][col] * x[col];
        }
        x[row] /= A[row][row];
    }
}

void back_sub_col_serial(float **A, float *b, float *x, int n){
    for (int row = 0; row<n; row++)
        x[row] = b[row];

    for (int col=n-1; col>=0; col--) {
        x[col] /= A[col][col];
        for (int row = col-1; row >= 0; row--){
            x[row] -= A[row][col]*x[col];
        }
    }
}

//attempt at parallelizing the initialization
void init_parallel(float ***A, float **b, float **x, int size, int num_threads){
    *A = malloc(size * sizeof(float *)); MEMCHECK(A);
    float **matrix = *A;
    matrix[0] = calloc(size * size, sizeof(float)); MEMCHECK(matrix[0]);
    *b = malloc(size * sizeof(float)); MEMCHECK(b);
    *x = malloc(size * sizeof(float)); MEMCHECK(x);

    // Serial pointer initialization
    for (int i = 0; i < size; i++) {
        matrix[i] = matrix[0] + i * size;
    }

    // Parallel initialization of b and matrix elements
    #pragma omp parallel num_threads(num_threads) 
    {
        unsigned int seed = omp_get_thread_num();

        #pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            float tmp = rand_r(&seed) % 101;
            (*b)[i] = tmp == 50 ? 1 : tmp - 50;

            // Initialize row of A
            for (int j = i; j < size; j++) {
                float tmp = rand_r(&seed) % 101;
                matrix[i][j] = tmp == 50 ? 1 : tmp - 50;
            }
        }
    }
    
}

void back_sub_row_parallel(float **A, float *b, float *x, int n, int thread_num){
    #pragma omp parallel for num_threads(thread_num)
    for(int row=n-1; row >= 0; row--){
        x[row] = b[row];
        for(int col = row+1; col<n; col++)
            x[row] -= A[row][col] * x[col];
        
        x[row] /= A[row][row];
    }
}


void back_sub_col_parallel(float **A, float *b, float *x, int n, int thread_num){
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for
        for (int row = 0; row<n; row++)
            x[row] = b[row];

        for (int col = n-1; col>=0; col--) {
            #pragma omp single
            {
                x[col] /= A[col][col];
            }
            
            #pragma omp for
            for (int row = 0; row < col; row++)
                x[row] -= A[row][col]*x[col];
        }
    }
}


int main(int argc, char *argv[]) {
    int size = 8;
    int is_serial = 0;
    int is_col = 0;
    int thread_num = 4;

    if(argc != 1){
        if (argc > 5 || argc < 4) ERR_EXIT("Usage: ./ask2/back_sub <size> <par/ser> <row/col> <thread_num>\n");
        char *endptr;
        errno = 0;

        size = strtol(argv[1], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || size <= 0) {
            fprintf(stderr, "Invalid size: %s\n", argv[1]);
            return 1;
        }

        char *mode = argv[2];
        if (strcmp(mode, "ser") == 0) {
            is_serial = 1;
        } else if (strcmp(mode, "par") != 0) {
            fprintf(stderr, "Invalid mode: %s\n", mode);
            return 1;
        }

        char *operation = argv[3];
        if (strcmp(operation, "col") == 0) {
            is_col = 1;
        } else if (strcmp(operation, "row") != 0) {
            fprintf(stderr, "Invalid operation: %s\n", operation);
            return 1;
        }

        if(!is_serial){
            if(argc != 5) ERR_EXIT("Usage: ./ask2/back_sub <size> <par/ser> <row/col> <thread_num>\n");
            thread_num = strtol(argv[4], &endptr, 10);  
            if (errno != 0 || *endptr != '\0' || thread_num <= 0) {
                fprintf(stderr, "Invalid thread number: %s\n", argv[4]);
                return 1;
            }
        }
        
    }

    double start, finish;
    float **matrix, *b, *x;

    // init_serial(&matrix, &b, &x, size); SAVE TIME
    init_parallel(&matrix, &b, &x, size, thread_num);
    
    if(is_serial){
        GET_TIME(start);
        if(is_col){
            back_sub_col_serial(matrix, b, x, size);
        }
        else{
            back_sub_row_serial(matrix, b, x, size);
        }
        GET_TIME(finish);
    }
    else{
        init_parallel(&matrix, &b, &x, size, thread_num);
        GET_TIME(start);
        if(is_col){
            back_sub_col_parallel(matrix, b, x, size, thread_num);
        }
        else{
            back_sub_row_parallel(matrix, b, x, size, thread_num);
        }
        GET_TIME(finish);
    }

    #ifdef OUT
        for (int i = 0; i < size; i++) {
            printf("x[%d] = %.2f\n", i, x[i]);
        }
    #endif
    printf("time: %f\n", finish - start);

    free(matrix[0]);
    free(matrix);
    free(b);
    free(x);
    return 0;
}