#ifndef __THREADS_H__

typedef void (*ThreadFunction_t)(void *Arg);

typedef struct
{
    ThreadFunction_t Function;
    void *Arg;
} ThreadJob_t;

#endif
