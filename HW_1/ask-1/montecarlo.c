#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "my_rand.h"
#include "../timer.h"
#include "../utils.h"

// serial implementation of the montecarlo algorithm
double montecarlo_s(long long int num_of_throws)
{
    unsigned seed = 1, xi;
    xi = my_rand(&seed);
    
    long long int arrows = 0;
    for(long long int throw = 0; throw < num_of_throws; throw++){
        double x = my_drand(&xi);
        double y = my_drand(&xi);
        double dist_square = x * x + y * y;
        if(dist_square <= 1) 
            arrows += 1; 
    }
    
    return 4*arrows / ((double) num_of_throws);;
}


typedef struct thread_arg {
    long long int throws;
    long long int hits;
    int rank;
}thread_arg;

void* mc_thread(void* arg){
    thread_arg* t_arg = (thread_arg*)arg;
    
    long long int num_of_throws = t_arg->throws, arrows = 0;
    unsigned int seed = my_rand((unsigned int *) &t_arg->rank);
    for(long long int throw = 0; throw < num_of_throws; throw++){
        double x = my_drand(&seed);
        double y = my_drand(&seed);
        double dist_square = x * x + y * y;
        if(dist_square <= 1) 
            arrows += 1; 
    }

    t_arg->hits = arrows;
    return NULL;
}

// parallel implementation of the montecarlo algorithm
double montecarlo_p(long long int num_of_throws, long long int num_of_threads){
    pthread_t* threads = malloc(num_of_threads * sizeof(pthread_t)); MEMCHECK(threads);
    thread_arg* thread_args = malloc(num_of_threads * sizeof(thread_arg)); MEMCHECK(thread_args)

    long long int throws_per_thread = num_of_throws / num_of_threads, remainder = num_of_throws % num_of_threads;

    for(int i=0; i<num_of_threads; i++){
        // divide the number of throws equally for each thread
        thread_args[i].throws = throws_per_thread + (i < remainder ? 1 : 0);
        thread_args[i].rank = i+1;  //+1 to init thread with rank 0 to 1
        ERRL(pthread_create(&threads[i], NULL, mc_thread, &thread_args[i]), "pthread_create@main");
    }

    long long int arrows = 0;
    for (int i = 0; i < num_of_threads; i++) {
        ERRL(pthread_join(threads[i], NULL), "pthread_join@main");
        arrows += thread_args[i].hits;
    }

    free(threads);
    free(thread_args);
    return 4*arrows / ((double) num_of_throws);
    
}

int main(int argc, char** argv){

    int r = 0;
    if(argc < 3 || argc > 4) ERR_EXIT("Usage: ./montecarlo <num_of_throws> <num_of_threads> [r]]\n");
    else if(argc == 4 && strcmp("r", argv[3]) ) ERR_EXIT("invalid flag\n");
    else if(argc == 4 && !strcmp("r", argv[3])) r = 1;

    char* endptr;
    long long int num_of_throws = strtoull(argv[1], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid num_of_throws\n");
    }
    
    int num_of_threads = strtoull(argv[2], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid num_of_threads\n");
    }

    double start_s, finish_s , pi_s, \
            start_p, finish_p , pi_p;
    
    GET_TIME(start_s);
    pi_s = montecarlo_s(num_of_throws);
    GET_TIME(finish_s);
    
    GET_TIME(start_p);
    pi_p = montecarlo_p(num_of_throws, num_of_threads);
    GET_TIME(finish_p);

    if(r){
        printf("%lf\n", pi_s);
        printf("%lf\n", finish_s - start_s);
        printf("----------------------\n");
        printf("%lf\n", pi_p);
        printf("%lf\n", finish_p - start_p);
        
        return 0;
    }
    printf("serial pi %lf\n", pi_s);
    printf("serial time: %lf\n", finish_s - start_s);
    printf("parallel pi: %lf\n", pi_p);
    printf("parallel time: %lf\n", finish_p - start_p);
    return 0;
}



