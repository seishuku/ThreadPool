#include <stdio.h>
#include "threads.h"

typedef struct
{
    ThreadFunction_t Function;
    void *Arg;
} ThreadJob_t;

void *Thread_Worker(void *Data)
{
	ThreadWorker_t *Worker=(ThreadWorker_t *)Data;

	printf("Worker thread starting...\r\n");

	while(!Worker->Stop)
	{
		pthread_mutex_lock(&Worker->Mutex);
        for(uint32_t i=0;i<List_GetCount(&Worker->Jobs);i++)
		{
			ThreadJob_t *Job=List_GetPointer(&Worker->Jobs, i);

			Job->Function(Job->Arg);

			List_Del(&Worker->Jobs, i);
		}
		pthread_mutex_unlock(&Worker->Mutex);
	}

	printf("Worker thread done.\r\n");

	return 0;
}

void Thread_AddJob(ThreadWorker_t *Worker, ThreadFunction_t JobFunc, void *JobArg)
{
	pthread_mutex_lock(&Worker->Mutex);
	List_Add(&Worker->Jobs, &(ThreadJob_t) { JobFunc, JobArg });
	pthread_mutex_unlock(&Worker->Mutex);
}

bool Thread_Init(ThreadWorker_t *Worker)
{
	Worker->Stop=false;

	List_Init(&Worker->Jobs, sizeof(ThreadJob_t), 10, NULL);

	if(pthread_mutex_init(&Worker->Mutex, NULL))
	{
		printf("Unable to create mutex.\r\n");
		return false;
	}

	if(pthread_create(&Worker->Thread, NULL, Thread_Worker, (void *)Worker))
	{
		printf("Unable to create worker thread.\r\n");
		return false;
	}

	return true;
}

bool Thread_Destroy(ThreadWorker_t *Worker)
{
	Worker->Stop=true;

	pthread_join(Worker->Thread, NULL);
	pthread_mutex_destroy(&Worker->Mutex);

	List_Destroy(&Worker->Jobs);

	return true;
}
