#pragma once
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ERR_EXIT(msg) do { fputs(msg, stderr); fflush(stdout); exit(0); } while (0)
#define ERRL(LIBCALL, PERR){ \
    errno = LIBCALL; \
    if((errno) != 0){ \
        perror((PERR)); \
        exit(EXIT_FAILURE); \
    } \
}
#define MEMCHECK(PTR){ \
    if(PTR == NULL){ \
        fprintf(stderr,"No more memory, exiting..."); \
        exit(EXIT_FAILURE); \
    } \
}
