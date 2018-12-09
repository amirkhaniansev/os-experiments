/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Full notice : https://github.com/amirkhaniansev/os-experiments/tree/master/LICENSE
 */

#include <stdio.h>
#include <Windows.h>

#define BUFFER_COUNT             10
#define PRODUCERS_COUNT          5
#define CONSUMERS_COUNT          15
#define SEMAPHORE_MAX_COUNT      10
#define FULL_INITIAL_COUNT       0
#define EMPTY_INITIAL_COUNT      10
#define FULL                     1
#define EMPTY                    0
#define INVALID_ARGS_MSG         L"Invalid amount of arguments."
#define SEMAPHORE_ERROR          L"Unable to create semaphores."
#define MUTEX_ERROR              L"Unable to create mutex."
#define PRODUCER_MSG_FORMAT      L"Producer   %2d : %5x    filled cell %d. "
#define CONSUMER_MSG_FORMAT      L"Consumer   %2d : %5x   emptied cell %d. "
#define PRODUCER_CREATED_FORMAT  L"Producer   %2d : %5x   is created.\n"
#define CONSUMER_CREATED_FORMAT  L"Consumer   %2d : %5x   is created.\n"
#define BUFFER_STATE             L"Buffer State : "
#define BUFFER_ITEM_FORMAT       L" %d "
#define NEW_LINE                 L" \n"

HANDLE* producers = NULL;
HANDLE* consumers = NULL;
HANDLE  mutex = NULL;
HANDLE  empty_semaphore = NULL;
HANDLE  full_semaphore = NULL;
DWORD*  producer_ids = NULL;
DWORD*  consumer_ids = NULL;
int*    buffer = NULL;
int     enumerator = 0;

void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(L"%s\n", msg);
	exit(exit_code);
}

void init()
{
	empty_semaphore = CreateSemaphore(
		NULL, EMPTY_INITIAL_COUNT, SEMAPHORE_MAX_COUNT, NULL);

	full_semaphore = CreateSemaphore(
		NULL, FULL_INITIAL_COUNT, SEMAPHORE_MAX_COUNT, NULL);

	if (empty_semaphore == NULL || full_semaphore == NULL)
		exit_with_message(SEMAPHORE_ERROR, -1);

	mutex = CreateMutex(NULL, FALSE, NULL);

	if (mutex == NULL)
		exit_with_message(MUTEX_ERROR, -1);

	producers = malloc(sizeof(HANDLE) * PRODUCERS_COUNT);
	consumers = malloc(sizeof(HANDLE) * CONSUMERS_COUNT);
	producer_ids = malloc(sizeof(DWORD) * PRODUCERS_COUNT);
	consumer_ids = malloc(sizeof(DWORD) * CONSUMERS_COUNT);
	buffer = malloc(sizeof(int) * BUFFER_COUNT);
	
	ZeroMemory(buffer, BUFFER_COUNT * sizeof(int));
}

void finalize()
{
	for (int i = 0; i < PRODUCERS_COUNT; i++)
		CloseHandle(producers[i]);

	for (int i = 0; i < CONSUMERS_COUNT; i++)
		CloseHandle(consumers[i]);

	CloseHandle(empty_semaphore);
	CloseHandle(full_semaphore);
	CloseHandle(mutex);

	free(producers);
	free(consumers);
	free(producer_ids);
	free(consumer_ids);
	free(buffer);
}

void show_buffer_state()
{
	wprintf(BUFFER_STATE);

	for (int i = 0; i < BUFFER_COUNT; i++)
		wprintf(BUFFER_ITEM_FORMAT, buffer[i]);

	wprintf(NEW_LINE);
}

DWORD WINAPI producer_func(void* param)
{
	int producer_number = param;

	while (1)
	{	
		Sleep((rand() % 2) * 1000);

		WaitForSingleObject(empty_semaphore, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		buffer[enumerator] = FULL;

		wprintf(PRODUCER_MSG_FORMAT,producer_number,
			producer_ids[producer_number], enumerator++);

		show_buffer_state();

		ReleaseMutex(mutex);
		ReleaseSemaphore(full_semaphore, 1L, NULL);
	}

	return 0UL;
}

DWORD WINAPI consumer_func(void* param)
{
	int consumer_number = param;

	while (1)
	{
		Sleep((rand() % 2) * 1000);

		WaitForSingleObject(full_semaphore, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		buffer[enumerator] = EMPTY;

		wprintf(CONSUMER_MSG_FORMAT, consumer_number, 
			consumer_ids[consumer_number], enumerator--);

		show_buffer_state();

		ReleaseMutex(mutex);
		ReleaseSemaphore(empty_semaphore, 1L, NULL);	
	}

	return 0UL;
}

int wmain(int argc, wchar_t** argv)
{
	if (argc != 1)
		exit_with_message(INVALID_ARGS_MSG, -1);

	init();
	
	for (int i = 0; i < PRODUCERS_COUNT; i++)
	{
		producers[i] = CreateThread(
			NULL, 0, producer_func, i, 0, &producer_ids[i]);
	
		wprintf(PRODUCER_CREATED_FORMAT, i, producer_ids[i]);
	}

	for (int i = 0; i < CONSUMERS_COUNT; i++)
	{
		consumers[i] = CreateThread(
			NULL, 0, consumer_func, i, 0, &consumer_ids[i]);
	
		wprintf(CONSUMER_CREATED_FORMAT, i , consumer_ids[i]);
	}

	WaitForMultipleObjects(2, producers, TRUE, INFINITE);
	WaitForMultipleObjects(2, consumers, TRUE, INFINITE);

	finalize();

	return 0;
}