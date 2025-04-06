# Parallel_Systems

Contains 3 group assignments for the Parallel Systems course at NKUA. 

## HW_1 (Pthreads)

### Exercise 1.1 

In this exercise, we had to implement a Monte Carlo simulation to estimate the value of π. The simulation involved randomly throwing darts at a square target with a circle inscribed inside it. By calculating the ratio of darts that landed inside the circle to the total number of darts thrown, we could approximate π using the formula $π = 4 * numberOfDartsInsideCircle/numberOfThrows$. We first implemented a serial version of the algorithm and then a parallel version using Pthreads. Our program took as input the number of dart throws and the number of threads. Additionally, we measured and compared the execution times of the serial and parallel implementations to analyze the performance improvement with parallelization. We also considered the necessity of synchronization when updating shared variables in the parallel implementation. Finally, we compared the speedup achieved for different numbers of threads and dart throws.

### Exercise 1.2 

In this exercise, we had to implement a multithreaded program using Pthreads, where multiple threads update a shared variable. The shared variable was initialized to zero by the main thread, and each thread executed a loop with a fixed number of iterations, incrementing the shared variable in each iteration. Since multiple threads were modifying the same variable simultaneously, data races could occur. To ensure correct execution, we implemented two different synchronization approaches: (1) using pthread locks (mutexes) and (2) using atomic operations. Finally, we compared the performance of both approaches for different numbers of iterations and threads, analyzing any differences in execution time and explaining the reasons behind them.

### Exercise 1.3

Here, we had to modify our previous multithreaded program to use a shared array instead of a single shared variable. The number of elements in the array was equal to the number of threads, and each thread was responsible for incrementing only one specific element of the array. The array was initialized to zero by the main thread, and each thread executed a loop with a fixed number of iterations, incrementing its assigned array element in each iteration. After all threads finished execution, the main thread printed the final values of the array elements and their sum, which had to be deterministic.

Since each thread updated a separate element of the array, synchronization was not necessary, as there was no data contention between threads. We ensured that the total number of updates to the array remained constant as the number of threads increased and compared the program’s performance across different thread counts. We analyzed whether performance improved with more threads and explored potential optimizations to further enhance efficiency.

### Exercise 1.4

In this exercise, we had to implement our own reader-writer locks using Pthreads. Our implementation used a data structure containing two condition variables—one for reader threads and one for writer threads—along with a mutex and counters to track the number of active and waiting readers and writers. The mutex ensured safe access to the shared data structure, and each thread had to acquire the appropriate lock before proceeding.

We implemented two different approaches: one prioritizing reader threads and the other prioritizing writer threads. We then modified a given linked list program to use our custom locks instead of Pthreads' built-in reader-writer locks. Finally, we compared the performance of both approaches under different thread counts and operation distributions, analyzing how prioritizing readers or writers affected efficiency.

## HW_2 (OpenMP)

### Exercise 2.1

In this exercise, the goal is to implement Conway’s Game of Life both sequentially and in parallel using OpenMP. The game runs on a 2D grid (NxN), where each cell is either "alive" or "dead," and its next state is determined by the state of its eight neighbors. The simulation evolves in discrete steps (generations), with all updates happening simultaneously.

The program should accept the following input parameters:
- the number of generations to simulate,
- the size of the grid,
- the execution mode (sequential or parallel), and
- the number of threads to use.

Additionally, we are asked to run the simulation for 1000 generations using different grid sizes (64x64, 1024x1024, 4096x4096) and varying numbers of threads, and then evaluate the performance.

### Exercise 2.2

This exercise focuses on solving an upper triangular linear system using backward substitution. Two versions of the algorithm are provided: one that traverses the system "row-wise" and one "column-wise." The tasks include:
- Analyzing whether the outer and inner loops of each version can be parallelized using OpenMP.
- Implementing both algorithms in sequential and parallel form.
- Creating a program that initializes an upper triangular matrix A, a vector b (right-hand side), and solves the system using the chosen method.

The program should accept as input:
- the size n of the linear system,
- whether to use the sequential or parallel version,
- whether to use the row-wise or column-wise algorithm, and
- the number of threads to use.

Finally, the exercise involves evaluating the performance and scalability of both algorithms for large matrix sizes (e.g., n = 10000). Optionally, the impact of different OpenMP scheduling strategies (schedule(runtime)) should be tested to identify which schedule yields the best performance.

## HW_3 (MPI)

### Exercise 3.1

In this exercise, we are asked to implement Conway’s Game of Life using MPI, based on the sequential algorithm developed in Exercise 2.1 (without OpenMP). The program should take the following input parameters:
- the number of generations,
- and the size of the grid.

The parallelization follows the master-worker model:
- Process 0 is responsible for creating and initializing the grid.
- It then distributes the necessary portions of the grid to the other MPI processes so they can perform the computation in parallel.
- After the simulation completes, process 0 gathers the results (final grid) and prints the outcome.

We are also required to run the program for 1000 generations, experimenting with various grid sizes and numbers of processes, and present the corresponding performance results.

### Exercise 3.2

In this exercise, we are required to implement a parallel matrix-vector multiplication using MPI, where the matrix is distributed among the processes by block-columns. The setup is as follows:
- Process 0 generates a square matrix of size n x n and a vector of size n.
- It then distributes only the required parts of the matrix and the vector to the other MPI processes.
- Each process performs part of the multiplication independently, and the results are then collected.

The program should take as input:
- the matrix size n, which must be evenly divisible by the number of processes (comm_sz).

After implementing the parallel algorithm, we evaluate its performance across different numbers of processes and matrix sizes. The goal is to compare the execution time against the sequential version and observe the achieved speedup. The report should also explain why speedup is or isn’t observed, considering computation and communication overheads.

## Group Members
- Marinaki Themis 
- Ioannis Ksiros

