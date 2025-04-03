#include <string.h>
#include "rw_lock.h"
#include "../utils.h"

// lock initializer
void rw_init(rw_lock* lock){
    memset(lock, 0, sizeof(rw_lock));
    ERRL(pthread_mutex_init(&lock->mutex, NULL), "mutex_init@rw_init");
    ERRL(pthread_cond_init(&lock->cond_rd, NULL), "cond_init@rw_init");
    ERRL(pthread_cond_init(&lock->cond_wr, NULL), "cond_init@rw_init");
}

// lock destroyer
void rw_destroy(rw_lock* lock){
    ERRL(pthread_mutex_destroy(&lock->mutex), "mutex_destroy@rw_init");
    ERRL(pthread_cond_destroy(&lock->cond_rd), "cond_destroy@rw_init");
    ERRL(pthread_cond_destroy(&lock->cond_wr), "cond_destroy@rw_init");
}

// implementation with writer priority
#ifdef WPRIO

// reader wants to acquire lock
void rw_read_acquire(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_read_acquire");
    // if there are active or waiting writers wait
    if(lock->active_wr == 1 || lock->wait_wr > 0){
        lock->wait_rd++;
        while(lock->active_wr == 1 || lock->wait_wr > 0){
            ERRL(pthread_cond_wait(&lock->cond_rd, &lock->mutex), "cond_wait@rw_read_acquire");
        }
        lock->wait_rd--;
    }
    lock->active_rd++;
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_lock@rw_read_acquire");
}

// writer wants to acquire lock
void rw_write_acquire(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_write_acquire");
    // if there are active writers or readers wait
    if(lock->active_wr == 1 || lock->active_rd > 0){
        lock->wait_wr++;
        while(lock->active_wr == 1 || lock->active_rd > 0){
            ERRL(pthread_cond_wait(&lock->cond_wr, &lock->mutex), "cond_wait@rw_write_acquire");
        }
        lock->wait_wr--;
    }
    lock->active_wr++;
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_lock@rw_write_acquire");
}

// lock release
void rw_release(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_release");
    // if a writer releases the lock
    if(lock->active_wr == 1){
        lock->active_wr--;
        // wake up a waiting writer or wake up all waiting readers
        if(lock->wait_wr){
            ERRL(pthread_cond_signal(&lock->cond_wr), "cond_signal@rw_release");
            ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_unlock@rw_release");
            return;
        }
        ERRL(pthread_cond_broadcast(&lock->cond_rd), "cond_broadcast@rw_release");
    }
    // else if a reader releases the lock
    else if(lock->active_rd > 0){
        lock->active_rd--;
        // if it is the last reader and we have waiting writers
        if(lock->active_rd == 0 && lock->wait_wr > 0){
            ERRL(pthread_cond_signal(&lock->cond_wr), "cond_signal@rw_release");
        }
    }
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_unlock@rw_release");
}

// implementation with reader priority
#else
// if a reader tries to acquire the lock
void rw_read_acquire(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_read_acquire");
    if(lock->active_wr == 1){
        lock->wait_rd++;
        while(lock->active_wr == 1){
            ERRL(pthread_cond_wait(&lock->cond_rd, &lock->mutex), "cond_wait@rw_read_acquire");
        }
        lock->wait_rd--;
    }
    lock->active_rd++;
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_lock@rw_read_acquire");
}

// if a writer tries to acquire the lock
void rw_write_acquire(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_write_acquire");
    if(lock->active_wr == 1 || lock->active_rd > 0){
        lock->wait_wr++;
        while(lock->active_wr == 1 || lock->active_rd > 0){
            ERRL(pthread_cond_wait(&lock->cond_wr, &lock->mutex), "cond_wait@rw_write_acquire");
        }
        lock->wait_wr--;
    }
    lock->active_wr++;
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_lock@rw_write_acquire");
}

void rw_release(rw_lock* lock){
    ERRL(pthread_mutex_lock(&lock->mutex), "mutex_lock@rw_release");
    if(lock->active_wr == 1){
        lock->active_wr--;
        if(lock->wait_rd){
            ERRL(pthread_cond_broadcast(&lock->cond_rd), "cond_broadcast@rw_release");
            ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_unlock@rw_release");
            return;
        }
        ERRL(pthread_cond_signal(&lock->cond_wr), "cond_signal@rw_release");
    }
    else if(lock->active_rd > 0){
        lock->active_rd--;
        if(lock->active_rd == 0 && lock->wait_wr > 0){
            ERRL(pthread_cond_signal(&lock->cond_wr), "cond_signal@rw_release");
        }
    }
    ERRL(pthread_mutex_unlock(&lock->mutex), "mutex_unlock@rw_release");
}
#endif