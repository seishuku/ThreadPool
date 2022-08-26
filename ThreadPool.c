#include <pthread.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils/list.h"
#include "threads.h"

bool Stop=false;
List_t Jobs;

pthread_t Thread;
pthread_mutex_t Mutex=PTHREAD_MUTEX_INITIALIZER;

static void *Thread_Worker(void *data)
{
    printf("Worker thread starting...\n");

    while(!Stop)
    {
        for(uint32_t i=0;i<List_GetCount(&Jobs);i++)
        {
			pthread_mutex_lock(&Mutex);
			ThreadJob_t *Job=List_GetPointer(&Jobs, i);

			Job->Function(Job->Arg);

			List_Del(&Jobs, i);
			pthread_mutex_unlock(&Mutex);
		}
	}

    printf("Worker thread done.\n");

	return 0;
}

void Thread_AddJob(ThreadFunction_t JobFunc, void *JobArg)
{
	pthread_mutex_lock(&Mutex);
	List_Add(&Jobs, &(ThreadJob_t) { JobFunc, JobArg });
	pthread_mutex_unlock(&Mutex);
}

void Job1(void *Arg)
{
	printf("Job 1 doing things!\n");
	for(volatile uint32_t i=0;i<100000000;i++);
	printf("Job 1 done things!\n");
}

void Job2(void *Arg)
{
	printf("Job 2 doing things!\n");
	for(volatile uint32_t i=0;i<100000000;i++);
	printf("Job 2 done things!\n");
}

int main()
{
	bool Done=false;

	List_Init(&Jobs, sizeof(ThreadJob_t), 10, NULL);

	if(pthread_mutex_init(&Mutex, NULL))
	{
		return -1;
	}

	if(pthread_create(&Thread, NULL, Thread_Worker, NULL))
	{
		printf("Unable to create worker thread.\n");
		return -1;
	}

	printf("Starting...\n\n");
	while(!Done)
	{
		printf("Number of jobs: %d                   \r", (uint32_t)List_GetCount(&Jobs));

		if(_kbhit())
		{
			switch(getch())
			{
				case 'q':
					Done=true;
					break;

				case 'a':
					Thread_AddJob(Job1, NULL);
					break;

				case 'b':
					Thread_AddJob(Job2, NULL);
					break;

				default:
					break;
			}
		}
	}

	Stop=true;

	pthread_join(Thread, NULL);
	pthread_mutex_destroy(&Mutex);

	List_Destroy(&Jobs);
}
