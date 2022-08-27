#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "threads.h"

void Job1(void *Arg)
{
	printf("Job 1 doing things!\r\n");
	for(uint32_t i=0;i<1000000;i++)
	{
		float sq=sqrtf((float)i);
		printf("%0.4f\r", sq);
	}
	printf("Job 1 done things!\r\n");
}

void Job2(void *Arg)
{
	printf("Job 2 doing things!\r\n");
	for(uint32_t i=0;i<1000000;i++)
	{
		float sq=sqrtf((float)i);
		printf("%0.4f\r", sq);
	}
	printf("Job 2 done things!\r\n");
}

void Thread1Constructor(void *Arg)
{
	printf("Worker thread 1 starting...\r\n");
}

void Thread1Destructor(void *Arg)
{
	printf("Worker thread 1 stopping...\r\n");
}

void Thread2Constructor(void *Arg)
{
	printf("Worker thread 2 starting...\r\n");
}

void Thread2Destructor(void *Arg)
{
	printf("Worker thread 2 stopping...\r\n");
}

int main()
{
	bool Done=false;
	ThreadWorker_t Worker, Worker2;

#ifndef WIN32
	initscr();
	clear();
	noecho();
	cbreak();
#endif

	Thread_Init(&Worker);
	Thread_AddConstructor(&Worker, Thread1Constructor, NULL);
	Thread_AddDestructor(&Worker, Thread1Destructor, NULL);
	Thread_Start(&Worker);

	Thread_Init(&Worker2);
	Thread_AddConstructor(&Worker2, Thread2Constructor, NULL);
	Thread_AddDestructor(&Worker2, Thread2Destructor, NULL);
	Thread_Start(&Worker2);

	printf("Starting...\r\n\n");
	while(!Done)
	{
		printf("Number of jobs: %d\r", (uint32_t)List_GetCount(&Worker.Jobs));

#ifdef WIN32
		if(!_kbhit())
		{
#endif
		switch(getch())
		{
			case 'q':
				Done=true;
				break;

			case 'a':
				Thread_AddJob(&Worker, Job1, NULL);
				break;

			case 'b':
				Thread_AddJob(&Worker2, Job2, NULL);
				break;

			default:
				break;
		}
#ifdef WIN32
		}
#endif
	}

	Thread_Destroy(&Worker);
	Thread_Destroy(&Worker2);

#ifndef WIN32
	endwin();
#endif
}
