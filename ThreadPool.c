#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "threads.h"

#ifdef WIN32
#include <Windows.h>
#define DBGPRINTF(...) { char buf[512]; snprintf(buf, sizeof(buf), __VA_ARGS__); OutputDebugString(buf); }
#else
#define DBGPRINTF(...) { fprintf(stderr, __VA_ARGS__); }
#endif

HWND hWnd, hProgress1, hProgress2, hStatic1, hStatic2;

bool Done=false;
ThreadWorker_t Worker, Worker2;

void Job1(void *Arg)
{
	for(uint32_t i=0;i<100;i++)
	{
		SetScrollPos(hProgress1, SB_CTL, i, TRUE);
		Sleep(1);
	}
}

void Job2(void *Arg)
{
	for(uint32_t i=0;i<100;i++)
	{
		SetScrollPos(hProgress2, SB_CTL, i, TRUE);
		Sleep(1);
	}
}

void ThreadConstructor(void *Arg)
{
	DBGPRINTF("Worker thread %lld starting...\r\n", pthread_self());
}

void ThreadDestructor(void *Arg)
{
	DBGPRINTF("Worker thread %lld stopping...\r\n", pthread_self());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CREATE:
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			break;

		case WM_SIZE:
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case 'P':
					Thread_Pause(&Worker);
					break;
				case 'O':
					Thread_Resume(&Worker);
					break;

				case 'A':
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					Thread_AddJob(&Worker, Job1, NULL);
					break;

				case 'B':
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					Thread_AddJob(&Worker2, Job2, NULL);
					break;

				case 'Q':
					PostQuitMessage(0);
					break;

				default:
					break;
			}
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	WNDCLASS wc;
	wc.style=CS_VREDRAW|CS_HREDRAW|CS_OWNDC;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=GetModuleHandle(NULL);
	wc.hIcon=LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground=GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName="szAppName";

	RegisterClass(&wc);

	RECT WindowRect;
	WindowRect.left=0;
	WindowRect.right=400;
	WindowRect.top=0;
	WindowRect.bottom=80;

	AdjustWindowRect(&WindowRect, WS_POPUPWINDOW|WS_CAPTION, FALSE);

	hWnd=CreateWindow("szAppName", "szAppName", WS_POPUPWINDOW|WS_CAPTION|WS_VISIBLE|WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, hInstance, NULL);
	hProgress1=CreateWindow("SCROLLBAR", NULL, SBS_HORZ|WS_CHILD|WS_VISIBLE, 0, 0, 400, 20, hWnd, NULL, hInstance, NULL);
	hProgress2=CreateWindow("SCROLLBAR", NULL, SBS_HORZ|WS_CHILD|WS_VISIBLE, 0, 20, 400, 20, hWnd, NULL, hInstance, NULL);
	hStatic1=CreateWindow("STATIC", "Bla", WS_CHILD|WS_VISIBLE, 0, 40, 400, 20, hWnd, NULL, hInstance, NULL);
	hStatic2=CreateWindow("STATIC", "Bla", WS_CHILD|WS_VISIBLE, 0, 60, 400, 20, hWnd, NULL, hInstance, NULL);

	SetScrollRange(hProgress1, SB_CTL, 0, 100, TRUE);
	SetScrollRange(hProgress2, SB_CTL, 0, 100, TRUE);

	Thread_Init(&Worker);
	Thread_AddConstructor(&Worker, ThreadConstructor, NULL);
	Thread_AddDestructor(&Worker, ThreadDestructor, NULL);
	Thread_Start(&Worker);

	Thread_Init(&Worker2);
	Thread_AddConstructor(&Worker2, ThreadConstructor, NULL);
	Thread_AddDestructor(&Worker2, ThreadDestructor, NULL);
	Thread_Start(&Worker2);

	while(!Done)
	{
		MSG Msg;

		if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			if(Msg.message==WM_QUIT)
				Done=1;
			else
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}
		else
		{
			char temp[512];

			sprintf(temp, "Thread 1 jobs: %d", Thread_GetJobCount(&Worker));
			SetWindowText(hStatic1, temp);
			sprintf(temp, "Thread 2 jobs: %d", Thread_GetJobCount(&Worker2));
			SetWindowText(hStatic2, temp);
		}
	}

	Thread_Destroy(&Worker);
	Thread_Destroy(&Worker2);

	DestroyWindow(hWnd);
}
