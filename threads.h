#ifndef __THREADS_H__

typedef void (*ThreadCallback)(void *Arg);

typedef struct
{
    ThreadCallback Function;
    void *Arg;
} ThreadJob_t;

#endif
