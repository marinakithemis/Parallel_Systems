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

// thread struct for implementation with locks
typedef struct thread_arg_l {
    long long int* shared_var;
    long long int iterations;  
    pthread_mutex_t* mutexaki;
}thread_arg_l;

// thread struct for implementation with atomic instructions
typedef struct thread_arg_a {
    long long int* shared_var;
    long long int iterations;
}thread_arg_a;

void* thread_l(void* arg){
    thread_arg_l* t_arg = (thread_arg_l*)arg;

    for(long long int it = 0; it < t_arg->iterations; it++){
        ERRL(pthread_mutex_lock(t_arg->mutexaki), "mutex_lock@thread");
        *(t_arg->shared_var) += 1;
        ERRL(pthread_mutex_unlock(t_arg->mutexaki), "mutex_unlock@thread");
    }

    return NULL;
}

void* thread_a(void* arg){
    thread_arg_a* t_arg = (thread_arg_a*)arg;

    for(long long int it = 0; it < t_arg->iterations; it++){
        __atomic_fetch_add(t_arg->shared_var, 1, 0);
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
        ERR_EXIT("Invalid num_of_threads\n");
    }
    
    int num_of_threads = strtoull(argv[2], &endptr, 0);
    if(*endptr != '\0'){
        ERR_EXIT("Invalid num_of_iterations\n");
    }

    double finish_1, start_1, finish_2, start_2;
    long long int iters_per_thread = iterations / num_of_threads, remainder = iterations % num_of_threads;

    // implementation with locks
    GET_TIME(start_1);
    pthread_t* threads = malloc(num_of_threads * sizeof(pthread_t)); MEMCHECK(threads);
    thread_arg_l* thread_args_l = malloc(num_of_threads * sizeof(thread_arg_l)); MEMCHECK(thread_args_l);
    pthread_mutex_t mutexaki;
    ERRL(pthread_mutex_init(&mutexaki, NULL), "mutex_init@main");

    long long int shared_val  = 0;
    for(int i=0; i<num_of_threads; i++){
        thread_args_l[i].shared_var = &shared_val;
        thread_args_l[i].iterations = iters_per_thread + (i < remainder ? 1 : 0);
        thread_args_l[i].mutexaki = &mutexaki;
        ERRL(pthread_create(&threads[i], NULL, thread_l, &thread_args_l[i]), "pthread_create@main");
    }
    
    for (int i = 0; i < num_of_threads; i++) {
        ERRL(pthread_join(threads[i], NULL), "pthread_join@main");
    }
    free(threads);
    free(thread_args_l);
    GET_TIME(finish_1);

    // implementation with atomic instructions
    GET_TIME(start_2);
    threads = malloc(num_of_threads * sizeof(pthread_t)); MEMCHECK(threads);
    thread_arg_a* thread_args_a = malloc(num_of_threads * sizeof(thread_arg_a)); MEMCHECK(thread_args_a);
    
    printf("Shared value: %lld\n", shared_val);

    shared_val  = 0;
    for(int i=0; i<num_of_threads; i++){
        thread_args_a[i].shared_var = &shared_val;
        thread_args_a[i].iterations = iters_per_thread + (i < remainder ? 1 : 0);
        ERRL(pthread_create(&threads[i], NULL, thread_a, &thread_args_a[i]), "pthread_create@main");
    }
    
    for (int i = 0; i < num_of_threads; i++) {
        ERRL(pthread_join(threads[i], NULL), "pthread_join@main");
    }
    free(threads);
    free(thread_args_a);
    GET_TIME(finish_2);

    printf("Shared value: %lld\n", shared_val);
    
    if(r){
        printf("%lf\n", finish_1 - start_1);
        printf("%lf\n", finish_2 - start_2);
        
        return 0;
    }
    printf("Time with pthread mutex: %lf\n", finish_1 - start_1);
    printf("Time with atomic instructions: %lf\n", finish_2 - start_2);

    return 0;

}


