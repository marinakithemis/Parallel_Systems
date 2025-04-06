#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    int N, generations;

    int rank, comm_sz, rows_per_proc, extra_rows, row_width;
    int *grid = NULL, *local_grid = NULL, *grid_copy = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int exit_flag = 0;

    // Argument initialization by the Master process
    if (rank == 0) {
        if (argc > 1) {
            if (argc != 3) {
                fprintf(stderr, "Usage: ./game_of_life <grid_size> <generations>\n");
                exit_flag = 1;
            } 
            else {
                char *endptr;
                N = strtoul(argv[1], &endptr, 0);
                if (*endptr != '\0') {
                    fprintf(stderr, "Invalid grid size\n");
                    exit_flag = 1;
                }

                if (exit_flag == 0) {
                    generations = strtoul(argv[2], &endptr, 0);
                    if (*endptr != '\0') {
                        fprintf(stderr, "Invalid number of generations\n");
                        exit_flag = 1;
                    }
                }
            }
        } else {
            // Default values
            N = 7;
            generations = 5;
        }
    }

    MPI_Bcast(&exit_flag, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (exit_flag) {
        MPI_Finalize();
        return 1;
    }

    MPI_Bcast(&generations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    
    // Master process initializes the grid
    if (rank == 0) {
        row_width = N+2; // Include halo cols
        grid = calloc(N * row_width, sizeof(int));
        for (int i = 0; i < N; i++) {
            for (int j = 1; j < row_width-1; j++) {
                grid[i * row_width + j] = rand() % 2;
            }
        }
    }
    MPI_Bcast(&row_width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    

    // Divide rows as fairly as possible among processes
    rows_per_proc = N / comm_sz;
    extra_rows = N % comm_sz;

    int local_rows = rows_per_proc + (rank < extra_rows ? 1 : 0);
    //+ 2 to include halo cols
    local_grid = calloc((local_rows + 2) * row_width , sizeof(int));  
    grid_copy = calloc((local_rows + 2) * row_width, sizeof(int));

    // scatter grid rows to all processes
    double start_s, finish_s, elapsed_s, s_elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    start_s = MPI_Wtime();
    int *sendcounts = NULL, *displs = NULL;
    if (rank == 0) {
        sendcounts = malloc(comm_sz * sizeof(int));
        displs = malloc(comm_sz * sizeof(int));
        int offset = 0;
        for (int i = 0; i < comm_sz; i++) {
            sendcounts[i] = (rows_per_proc + (i < extra_rows ? 1 : 0)) * row_width;
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }
    MPI_Scatterv(grid, sendcounts, displs, MPI_INT, local_grid + row_width, local_rows*row_width, MPI_INT, 0, MPI_COMM_WORLD);
    finish_s = MPI_Wtime();
    elapsed_s = finish_s - start_s;
    MPI_Reduce(&elapsed_s, &s_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Scatter: %f\n", s_elapsed);
    }

    double local_start, local_finish, elapsed, local_elapsed;
    double start_g, finish_g, elapsed_g, g_elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    
    for(int gen=0; gen<generations; gen++) {
        //Exchange rows
        if (rank == 0 && comm_sz != 1) {
            MPI_Sendrecv(local_grid + (local_rows) * row_width, row_width, MPI_INT, rank + 1, 0, local_grid + row_width*(local_rows+1), row_width, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (rank == comm_sz - 1 && comm_sz != 1) {
            MPI_Sendrecv(local_grid + row_width, row_width, MPI_INT, rank - 1, 0, local_grid, row_width, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (comm_sz != 1) {
            MPI_Sendrecv(local_grid + (local_rows) * row_width, row_width, MPI_INT, rank + 1, 0, local_grid + row_width*(local_rows+1), row_width, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Sendrecv(local_grid + row_width, row_width, MPI_INT, rank - 1, 0, local_grid, row_width, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        //Compute the next generation
        for(int i=1; i<local_rows+1; i++) {
            for(int j=1; j<row_width-1; j++) {

                int alive_neighbours = local_grid[(i-1) * row_width + j] + local_grid[(i-1) * row_width + (j-1)] \
                + local_grid[i * row_width + (j-1)] + local_grid[(i+1) * row_width + j] + local_grid[(i+1) * row_width + (j+1)] \
                + local_grid[i * row_width + (j+1)] + local_grid[(i-1) * row_width + (j+1)] + local_grid[(i+1) * row_width + (j-1)];

                grid_copy[i * row_width + j] = local_grid[i * row_width + j] & (alive_neighbours == 2 || alive_neighbours == 3);
                grid_copy[i * row_width + j] |= (alive_neighbours == 3);
            }
        }

        int *temp = local_grid;
        local_grid = grid_copy;
        grid_copy = temp;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Gather results back to master
    MPI_Barrier(MPI_COMM_WORLD);
    start_g = MPI_Wtime();
    MPI_Gatherv(local_grid + row_width, local_rows * row_width, MPI_INT, grid, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    finish_g = MPI_Wtime();
    elapsed_g = finish_g - start_g;
    MPI_Reduce(&elapsed_g, &g_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


    // Master prints the final grid
    if (rank == 0) {
        #ifdef OUT
            puts("Final grid:");
            for (int i = 0; i < N; i++) {
                for (int j = 1; j < row_width-1; j++) {
                    printf("%d ", grid[i * row_width + j]);
                }
                puts("");
            }
        #endif
        printf("Algorithm: %f\n", elapsed);
        printf("Gather: %f\n", g_elapsed);
        free(grid);
        free(sendcounts);
        free(displs);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    free(local_grid);
    free(grid_copy);
    MPI_Finalize();
    return 0;
}
