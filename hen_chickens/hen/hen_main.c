/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Full notice : https://github.com/amirkhaniansev/os-experiments/tree/master/LICENSE
 */

#include <stdio.h>
#include <Windows.h>

#define MSG_PATTERN          L"%s\n"
#define INVALID_ARG_MSG      L"Invalid amount of arguments."
#define FILL_MESSAGE         L"Hen filled %5d slices of food into the food dish.\n"
#define FULL                 1

// exits with the given exit code after printing given message to the console
void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(MSG_PATTERN, msg);
	exit(exit_code);
}

// entry point for hen process
int wmain(int argc, wchar_t** argv)
{
	if (argc != 5)
		exit_with_message(INVALID_ARG_MSG, -1);

	HANDLE map = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, argv[1]);
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, argv[2]);
	HANDLE full_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, argv[3]);
	HANDLE empty_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, argv[4]);

	unsigned char* view = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	unsigned char amount = 0;

	while (1)
	{
		WaitForSingleObject(empty_semaphore, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		
		if (view != NULL)
		{
			amount = view[0];

			view[1] = 2;

			for (int i = 2; i < amount; i++)
			{
				view[i] = FULL;
			}

			wprintf(FILL_MESSAGE, amount);
		}

		ReleaseMutex(mutex);
		ReleaseSemaphore(full_semaphore, amount, NULL);
	}

	UnmapViewOfFile(view);

	CloseHandle(map);
	CloseHandle(mutex);
	CloseHandle(full_semaphore);
	CloseHandle(empty_semaphore);

	return 0;
}