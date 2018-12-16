/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Full notice : https://github.com/amirkhaniansev/os-experiments/tree/master/LICENSE
 */

#include <stdio.h>
#include <Windows.h>

#define MSG_PATTERN          L"%s\n"
#define INVALID_ARG_MSG      L"Invalid amount of arguments."
#define EMPTY                0
#define CHICKEN_EAT_MSG      L"Chicken %5d  ate slice number  %5d  from food dish.\n"

// exits with the given exit code after printing given message to the console
void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(MSG_PATTERN, msg);
	exit(exit_code);
}

// entry point for hen process
int wmain(int argc, wchar_t** argv)
{
	if (argc != 6)
		exit_with_message(INVALID_ARG_MSG, -1);

	HANDLE map = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, argv[1]);
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, argv[2]);
	HANDLE full_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, argv[3]);
	HANDLE empty_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, argv[4]);
	int chicken_number = _wtoi(argv[5]);

	unsigned char* view = NULL;
	unsigned char amount = 0;
	unsigned char enumerator = 0;

	while (1)
	{
		Sleep((rand() % 2) * 1000);

		WaitForSingleObject(full_semaphore, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		view = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (view != NULL)
		{
			amount = view[0];
			enumerator = view[1];

			view[view[1]++] = EMPTY;

			wprintf(CHICKEN_EAT_MSG, chicken_number, enumerator - 2);
		}

		UnmapViewOfFile(view);

		ReleaseMutex(mutex);

		if (enumerator - 2 == amount - 1)
			ReleaseSemaphore(empty_semaphore, 1L, NULL);
	}

	CloseHandle(map);
	CloseHandle(mutex);
	CloseHandle(full_semaphore);
	CloseHandle(empty_semaphore);

	return 0;
}