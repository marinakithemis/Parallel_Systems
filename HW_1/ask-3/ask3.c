
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h> 
#include <unistd.h> 
#include <ctype.h> 
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "../timer.h"
#include "../utils.h"

typedef struct thread_arg {
    long long int* var;
    long long int iterations;  //readonly
}thread_arg;

void* thread(void* arg){
    thread_arg* t_arg = (thread_arg*)arg;

    for(long long int it = 0; it < t_arg->iterations; it++){
        (*t_arg->var)++;
    }

    return NULL;
}

int main(int argc, char* argv[]) {

    int r = 0;
    if(argc < 3 || argc > 4) ERR_EXIT("Usage: ./ask2 <num_of_iters> <num_of_threads> [r]\n");
    else if(argc == 4 && strcmp("r", argv[3]) ) ERR_EXIT("invalid flag\n");
    else if(argc == 4 && !strcmp("r", argv[3])) r = 1;

    char* endptr;
    long long int iterations = strtoull(argv[1], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid iterations\n");
    }
    
    int num_of_threads = strtoull(argv[2], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid num_of_threads\n");
    }

    double finish, start;
    long long int iters_per_thread = iterations / num_of_threads, remainder = iterations % num_of_threads;

    GET_TIME(start);
    pthread_t* threads = malloc(num_of_threads * sizeof(pthread_t)); MEMCHECK(threads);
    thread_arg* thread_args = malloc(num_of_threads * sizeof(thread_arg)); MEMCHECK(thread_args);
    long long int* shared_arr = calloc(num_of_threads, sizeof(long long int)); MEMCHECK(shared_arr);

    for(int i=0; i<num_of_threads; i++){
        thread_args[i].var = &shared_arr[i];
        thread_args[i].iterations = iters_per_thread + (i < remainder ? 1 : 0);
        ERRL(pthread_create(&threads[i], NULL, thread, &thread_args[i]), "pthread_create@main");
    }
    
    for (int i = 0; i < num_of_threads; i++) {
        ERRL(pthread_join(threads[i], NULL), "pthread_join@main");
    }

    free(threads);
    free(thread_args);
    GET_TIME(finish);
    
    long long int sum = 0;
    for(int i = 0; i < num_of_threads; i++) {
        sum += shared_arr[i];
    }    
    if(r){
        printf("%lld\n", sum);
        printf("%lf\n", finish - start);        
        free(shared_arr);
        return 0;
    }
      for(int i = 0; i < num_of_threads; i++) {
        printf("Value %d: %lld\n", i, shared_arr[i]);
    }
    printf("Sum of Values: %lld\n", sum);
    printf("Time with pthread mutex: %lf\n", finish - start);
    free(shared_arr);
    return 0;
}