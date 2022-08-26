#ifndef __THREADS_H__

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils/list.h"

typedef void (*ThreadFunction_t)(void *Arg);

typedef struct
{
	bool Stop;
	List_t Jobs;

	pthread_t Thread;
	pthread_mutex_t Mutex;
} ThreadWorker_t;

void Thread_AddJob(ThreadWorker_t *Worker, ThreadFunction_t JobFunc, void *JobArg);
bool Thread_Init(ThreadWorker_t *Worker);
bool Thread_Destroy(ThreadWorker_t *Worker);

#endif
