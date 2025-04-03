#pragma once
#include <pthread.h>


typedef struct rw_lock{
    pthread_mutex_t mutex;
    pthread_cond_t cond_rd;
    pthread_cond_t cond_wr;
    int active_rd;
    int wait_rd;
    int active_wr;
    int wait_wr;
}rw_lock;

void rw_init(rw_lock* lock);
void rw_destroy(rw_lock* lock);
void rw_read_acquire(rw_lock* lock);
void rw_write_acquire(rw_lock* lock);
void rw_release(rw_lock* lock);
