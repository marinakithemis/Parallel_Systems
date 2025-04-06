#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <errno.h>
#include <time.h>
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

void serial_algo(int **grid, int **grid_copy, int generations, int grid_size)
{
    for (int g = 0; g < generations; g++){
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

    }
}


int main(int argc, char* argv[]){
    
    int grid_size, generations; 
    char* endptr;

    if (argc > 1){
        if (argc != 3){
            ERR_EXIT("Usage: ./game_of_life <grid_size> <generations>\n");
        }
        grid_size = strtoul(argv[1], &endptr, 0) + 2;
        if(*endptr != '\0'){
            ERR_EXIT("Invalid grid size\n");
        }
        generations = strtoul(argv[2], &endptr, 0);
        if(*endptr != '\0'){
            ERR_EXIT("Invalid number of generations\n");
        }
    }
    // Default values
    else{
        grid_size = 7;
        generations = 5;
    }

    int **grid = (int**)malloc(grid_size * sizeof(int*)); MEMCHECK(grid);
    grid[0] = (int*)calloc(grid_size * grid_size, sizeof(int)); MEMCHECK(grid[0]);

    for (int i = 0; i < grid_size; i++){
        grid[i] = grid[0] + i * grid_size;
    }
    
    //initialize the grid randomly with 0 and 1
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

    double start, finish;
    
    GET_TIME(start);
    serial_algo(grid, grid_copy , generations, grid_size);
    GET_TIME(finish);
    
    printf("Algorithm: %f\n", finish - start);

    free(grid[0]);
    free(grid_copy[0]);
    free(grid);
    free(grid_copy);

    return 0;
}