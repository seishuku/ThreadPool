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

DWORD WINAPI Thread_Worker(_In_ LPVOID lpParameter)
{
    printf("Worker thread starting...\n");

    while(!Stop)
    {
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
	printf("Job1 doing things!\n");
	for(volatile uint32_t i=0;i<100000000;i++);
	printf("Job1 done things!\n");
}

void Job2(void *Arg)
{
	printf("Job1 doing things!\n");
	for(volatile uint32_t i=0;i<100000000;i++);
	printf("Job1 done things!\n");
}

int main()
{
	List_Init(&Jobs, sizeof(ThreadJob_t), 10, NULL);
	
	HANDLE thread=CreateThread(NULL, 0, Thread_Worker, NULL, 0, NULL);
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
					ThreadJob_t Job;
					Job.Function=Job1;
					Job.Arg=NULL;
					List_Add(&Jobs, &Job);
					break;
				}

				case 'b':
				{
					ThreadJob_t Job;
					Job.Function=Job2;
					Job.Arg=NULL;
					List_Add(&Jobs, &Job);
					break;
				}

				default:
					break;
			}
		}
	}

	Stop=true;
	CloseHandle(thread);
}
