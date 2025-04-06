#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <errno.h>
#include <time.h>
#include <omp.h>
#include "../utils.h"
#include "../timer.h"

void print_grid(int **grid, int grid_size)
{
    printf("\n");
    for (int i = 1; i < grid_size-1;i++){
        for(int j = 1; j < grid_size-1;j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

void serial_algo(int generations, int grid_size)
{
    int **grid = (int**)malloc(grid_size * sizeof(int*)); MEMCHECK(grid);
    grid[0] = (int*)calloc(grid_size * grid_size, sizeof(int)); MEMCHECK(grid[0]);

    for (int i = 0; i < grid_size; i++){
        grid[i] = grid[0] + i * grid_size;
    }
    
    //initialize the grid randomly with 0 and 1
    srand(time(NULL));
    for (int i = 1; i < grid_size-1; i++){
        for(int j = 1; j < grid_size-1; j++){
           grid[i][j] = rand() % 2 ;
        }
    }

    int **grid_copy = malloc(grid_size*sizeof(int *)); MEMCHECK(grid_copy);
    grid_copy[0] = malloc (grid_size * (grid_size) * sizeof(int)); MEMCHECK(grid_copy[0]);

    for (int i = 0; i < grid_size;i++){
      grid_copy[i] = grid_copy[0] + i * grid_size;
    }

    memcpy(grid_copy[0], grid[0], grid_size * grid_size * sizeof(int));

    for (int g = 0; g <= generations; g++){
        for(int i = 1; i < grid_size-1; i++){
            for(int j = 1; j < grid_size-1; j++){
                int alive_neighbours = grid[i-1][j] + grid[i-1][j-1] + grid[i][j-1] + grid[i + 1][j] + grid[i + 1][j + 1] + grid[i][j + 1] + grid[i - 1][j + 1] + grid[i + 1][j - 1];
                grid_copy[i][j] = grid[i][j] & (alive_neighbours == 2 || alive_neighbours == 3);
                grid_copy[i][j] |= (alive_neighbours == 3);
            }
        }
        int **temp = grid;
        grid = grid_copy;
        grid_copy = temp;


        #ifdef OUT
            print_grid(grid, grid_size);
        #endif

    }
    free(grid[0]);
    free(grid_copy[0]);
    free(grid);
    free(grid_copy);
}

void parallel_algo(int generations, int grid_size, int num_of_threads)
{
   int **grid = (int**)malloc(grid_size * sizeof(int*)); MEMCHECK(grid);
    grid[0] = (int*)calloc(grid_size * grid_size, sizeof(int)); MEMCHECK(grid[0]);

    for (int i = 0; i < grid_size; i++){
        grid[i] = grid[0] + i * grid_size;
    }

    //initialize the grid randomly with 0 and 1
    srand(time(NULL));
    for (int i = 1; i < grid_size-1; i++){
        for(int j = 1; j < grid_size-1; j++){
           grid[i][j] = rand() % 2 ;
        }
    }

    int **grid_copy = malloc(grid_size*sizeof(int *)); MEMCHECK(grid_copy);
    grid_copy[0] = malloc (grid_size * (grid_size) * sizeof(int)); MEMCHECK(grid_copy[0]);

    for (int i = 0; i < grid_size;i++){
      grid_copy[i] = grid_copy[0] + i * grid_size;
    }

    memcpy(grid_copy[0], grid[0], grid_size * grid_size * sizeof(int));
    int i, g, j;
    # pragma omp parallel num_threads(num_of_threads) \
            default(none) shared(grid_size, grid, grid_copy, generations) private(i, g, j)
    {
        for (g = 0; g <= generations; g++){
            # pragma omp for 
            for(i = 1; i < grid_size-1; i++){
                for(j = 1; j < grid_size-1; j++){
                    int alive_neighbours = grid[i-1][j] + grid[i-1][j-1] + grid[i][j-1] + grid[i + 1][j] + grid[i + 1][j + 1] + grid[i][j + 1] + grid[i - 1][j + 1] + grid[i + 1][j - 1];
                    grid_copy[i][j] = grid[i][j] & (alive_neighbours == 2 || alive_neighbours == 3);
                    grid_copy[i][j] |= (alive_neighbours == 3);
                }
            }
            # pragma omp single
            {
                int **temp = grid;
                grid = grid_copy;
                grid_copy = temp;
            }
        }
    }

    #ifdef OUT
        print_grid(grid, grid_size);
    #endif

    free(grid[0]);
    free(grid_copy[0]);
    free(grid);
    free(grid_copy);
}

int main(int argc, char* argv[])
{
    if(argc > 5 || argc < 4) ERR_EXIT("Usage: ./ask1/game_of_life <generations> <grid_size> <algo> <num_of_threads>\n");
    char* endptr;
    int generations = strtoull(argv[1], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid generations\n");
    }
    
    int grid_size = strtoull(argv[2], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid generations\n");
    }

    // 0 for serial, 1 for parallel algorithm
    int algo = strtoull(argv[3], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid generations\n");
    }

    int num_of_threads;
    if(algo) {
        if(argc != 5) ERR_EXIT("Usage: ./ask1/game_of_life <generations> <grid_size> <algo> <num_of_threads>\n");
        num_of_threads = strtoull(argv[4], &endptr, 0);
        if(*endptr != '\0'){
            ERR_EXIT("Invalid generations\n");
        }  
    }
    
    double start, finish;
    
    if (algo == 0){
        GET_TIME(start);
        serial_algo(generations, grid_size + 2);
        GET_TIME(finish);
        printf("time: %f\n", finish - start);
    }
    else if (algo == 1){
        GET_TIME(start);
        parallel_algo(generations, grid_size + 2, num_of_threads);
        GET_TIME(finish);
        printf("time: %f\n", finish - start);

    }
    else{
        ERR_EXIT("Invalid algo\n");
    }

}