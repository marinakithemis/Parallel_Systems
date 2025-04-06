#include <stdio.h>
#include <stdlib.h>
#include "../utils.h"
#include "../timer.h"

void print_matrix(float *matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6.2f ", matrix[i * n + j]);
        }
        printf("\n");
    }
}

void print_vector(float *vector, int size) {
    for (int i = 0; i < size; i++) {
        printf("%6.2f ", vector[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    int n;
    char* endptr;

    if (argc > 1){
        if (argc != 2){
            ERR_EXIT("Usage: ./mat_vec_serial <grid_size>\n");
        }
        n = strtoul(argv[1], &endptr, 0);
        if(*endptr != '\0'){
            ERR_EXIT("Invalid grid size\n");
        }
        else if (n <= 0) {
            ERR_EXIT("Error: n must be a positive integer\n");
        }
    }
    // Default values
    else{
        n = 8;
    }

    float *matrix = calloc(1, n * n * sizeof(float));
    float *vec = calloc(1, n * sizeof(float));
    float *result = calloc(1, n * sizeof(float));

    for (int i = 0; i < n; i++) {
        vec[i] = i;
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = i+j; // Fill with dummy data
        }
    }

    double start, finish;
    GET_TIME(start);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i * n + j] * vec[j];
        }
    }

    GET_TIME(finish);
    printf("Algorithm: %f\n", finish - start);

    // Clean up
    free(result);
    free(matrix);
    free(vec);

    return 0;
}
