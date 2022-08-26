#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils/list.h"
#include "threads.h"

bool Done=false;

bool Stop=false;
List_t Jobs;

LPCTSTR lpszMutex=L"ThreadWorkerMutex";
HANDLE Thread=NULL, Mutex=NULL;

DWORD WINAPI Thread_Worker(_In_ LPVOID lpParameter)
{
    printf("Worker thread starting...\n");

    while(!Stop)
    {
		WaitForSingleObject(Mutex, INFINITE);

        for(uint32_t i=0;i<List_GetCount(&Jobs);i++)
        {
            ThreadJob_t *Job=List_GetPointer(&Jobs, i);
			printf("running job...\n");
			Job->Function(Job->Arg);
			List_Del(&Jobs, i);
        }
    }

    printf("Worker thread done.\n");

	return 0;
}

void Job1(void *Arg)
{
	HANDLE exMutex=OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpszMutex);

	if(exMutex!=NULL)
	{
		WaitForSingleObject(exMutex, INFINITE);

		printf("Job 1 doing things!\n");
		for(volatile uint32_t i=0;i<100000000;i++);
		printf("Job 1 done things!\n");

		ReleaseMutex(exMutex);
	}
}

void Job2(void *Arg)
{
	HANDLE exMutex=OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpszMutex);

	if(exMutex!=NULL)
	{
		WaitForSingleObject(exMutex, INFINITE);

		printf("Job 2 doing things!\n");
		for(volatile uint32_t i=0;i<100000000;i++);
		printf("Job 2 done things!\n");

		ReleaseMutex(exMutex);
	}
}

int main()
{
	List_Init(&Jobs, sizeof(ThreadJob_t), 10, NULL);

	Mutex=CreateMutex(NULL, TRUE, lpszMutex);

	if(Mutex==NULL)
	{
		printf("Unable to create mutex.\n");
		return -1;
	}

	Thread=CreateThread(NULL, 0, Thread_Worker, NULL, 0, NULL);

	if(Thread==NULL)
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
				{
					HANDLE Lock=OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpszMutex);

					if(Lock!=NULL)
					{
						ThreadJob_t Job;
						Job.Function=Job1;
						Job.Arg=NULL;
						List_Add(&Jobs, &Job);

						ReleaseMutex(Lock);
					}
					break;
				}

				case 'b':
				{
					HANDLE Lock=OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpszMutex);

					if(Lock!=NULL)
					{
						ThreadJob_t Job;
						Job.Function=Job2;
						Job.Arg=NULL;
						List_Add(&Jobs, &Job);

						ReleaseMutex(Lock);
					}
					break;
				}

				default:
					break;
			}
		}
	}

	Stop=true;

	CloseHandle(Mutex);
	CloseHandle(Thread);
}
