#include <stdio.h>
#include "threads.h"

// Structure that holds the function pointer and argument
// to store in a list that can be iterated as a job list.
typedef struct
{
    ThreadFunction_t Function;
    void *Arg;
} ThreadJob_t;

// Main worker thread function, this does the actual calling of various job functions in the thread.
void *Thread_Worker(void *Data)
{
	// Get pointer to thread data
	ThreadWorker_t *Worker=(ThreadWorker_t *)Data;

	// If there's a constructor function assigned, call it.
	if(Worker->Constructor)
		Worker->Constructor(Worker->ConstructorArg);

	// Loop until stop is set
	while(!Worker->Stop)
	{
		// Lock mutex and work the current jobs (if any), then remove that job from the list.
		if(List_GetCount(&Worker->Jobs))
		{
			ThreadJob_t Job={ NULL, NULL };

			pthread_mutex_lock(&Worker->Mutex);
			List_GetCopy(&Worker->Jobs, 0, (void *)&Job);
			List_Del(&Worker->Jobs, 0);
			pthread_mutex_unlock(&Worker->Mutex);

			if(Job.Function)
				Job.Function(Job.Arg);
		}
	}

	// If there's a destructor funcrion assigned, call that.
	if(Worker->Destructor)
		Worker->Destructor(Worker->DestructorArg);

	return 0;
}

uint32_t Thread_GetJobCount(ThreadWorker_t *Worker)
{
	uint32_t Count=0;

	if(Worker)
	{
		pthread_mutex_lock(&Worker->Mutex);
		Count=(uint32_t)List_GetCount(&Worker->Jobs);
		pthread_mutex_unlock(&Worker->Mutex);
	}

	return Count;
}

// Adds a job function and argument to the job list.
void Thread_AddJob(ThreadWorker_t *Worker, ThreadFunction_t JobFunc, void *Arg)
{
	if(Worker)
	{
		pthread_mutex_lock(&Worker->Mutex);
		List_Add(&Worker->Jobs, &(ThreadJob_t) { JobFunc, Arg });
		pthread_mutex_unlock(&Worker->Mutex);
	}
}

// Assigns a constructor function and argument to the thread.
void Thread_AddConstructor(ThreadWorker_t *Worker, ThreadFunction_t ConstructorFunc, void *Arg)
{
	if(Worker)
	{
		Worker->Constructor=ConstructorFunc;
		Worker->ConstructorArg=Arg;
	}
}

// Assigns a destructor function and argument to the thread.
void Thread_AddDestructor(ThreadWorker_t *Worker, ThreadFunction_t DestructorFunc, void *Arg)
{
	if(Worker)
	{
		Worker->Destructor=DestructorFunc;
		Worker->DestructorArg=Arg;
	}
}

// Set up initial parameters and objects.
bool Thread_Init(ThreadWorker_t *Worker)
{
	if(Worker==NULL)
		return false;

	Worker->Stop=false;

	Worker->Constructor=NULL;
	Worker->ConstructorArg=NULL;

	Worker->Destructor=NULL;
	Worker->DestructorArg=NULL;

	List_Init(&Worker->Jobs, sizeof(ThreadJob_t), 10, NULL);

	if(pthread_mutex_init(&Worker->Mutex, NULL))
	{
		printf("Unable to create mutex.\r\n");
		return false;
	}

	return true;
}

// Starts up worker thread.
bool Thread_Start(ThreadWorker_t *Worker)
{
	if(Worker==NULL)
		return false;

	if(pthread_create(&Worker->Thread, NULL, Thread_Worker, (void *)Worker))
	{
		printf("Unable to create worker thread.\r\n");
		return false;
	}

	return true;
}

// Stops thread and waits for it to exit and destorys objects.
bool Thread_Destroy(ThreadWorker_t *Worker)
{
	if(Worker==NULL)
		return false;

	Worker->Stop=true;

	pthread_join(Worker->Thread, NULL);
	pthread_mutex_destroy(&Worker->Mutex);

	List_Destroy(&Worker->Jobs);

	return true;
}
