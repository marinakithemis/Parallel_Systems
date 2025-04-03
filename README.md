# Parallel_Systems

Contains 3 group assignments for the Parallel Systems course at NKUA. 

## Group Members
- Marinaki Themis - sdi2100096
- Ioannis Ksiros - sdi2200122

## HW_1

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

