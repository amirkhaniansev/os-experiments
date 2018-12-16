/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Full notice : https://github.com/amirkhaniansev/os-experiments/tree/master/LICENSE
 */

#include <stdio.h>
#include <Windows.h>

#define MSG_PATTERN          L"%s\n"
#define INVALID_ARG_MSG      L"Invalid amount of arguments."
#define FOOD_NAME            L"Food"
#define FOOD_MUTEX_NAME      L"Food_Mutex"
#define FOOD_ERROR           L"Unable to create food."
#define FOOD_AMOUNT_ERROR    L"Food amount must be less or equal than 253."
#define FOOD_VIEW_ERROR      L"Unable to create food view."
#define MUTEX_ERROR          L"Unable to create mutex."
#define SEMAPHORE_ERROR      L"Unable to create semaphore."
#define EMPTY_SEMAPHORE_NAME L"Empty_Semaphore"
#define FULL_SEMAPHORE_NAME  L"Full_Semaphore"
#define HEN_CMD_PATTERN      L"%s %s %s %s %s"
#define HEN_NAME             L"hen.exe"
#define HEN_CREATION_MSG     L"Hen is created."
#define CHICKEN_CMD_PATTERN  L"%s %s %s %s %s %d"
#define CHICKEN_NAME         L"chicken.exe"
#define HEN_ERROR            L"Unable to create hen."
#define CHICKEN_CREATION_MSG L"Chicken %d is created.\n"
#define SLEEP_TIME           20000

// exits with the given exit code after printing given message to the console
void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(MSG_PATTERN, msg);
	exit(exit_code);
}

// asserts the given boolean expression and
// exits with the given exit code after printing given message to the console 
// if the expression is false
void assert(_Bool expression, wchar_t* msg, int exit_code)
{
	if (!expression)
		exit_with_message(msg, exit_code);
}

// creates synchronization objetcs
void create_sync_objects(int amount_of_food)
{
	// creating mutex
	assert(CreateMutex(
		NULL, FALSE, FOOD_MUTEX_NAME) != NULL,
		MUTEX_ERROR, -1);

	// creating semaphores
	assert(CreateSemaphore(
		NULL, 0, amount_of_food, FULL_SEMAPHORE_NAME) != NULL,
		SEMAPHORE_ERROR, -1);

	assert(CreateSemaphore(
		NULL, 1, 1, EMPTY_SEMAPHORE_NAME) != NULL,
		SEMAPHORE_ERROR, -1);
}

// creates shared memory for food 
void create_food_mapping(int amount)
{
	HANDLE map_handle = CreateFileMapping(
		INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, amount, FOOD_NAME);

	assert(map_handle != NULL, FOOD_ERROR, -1);

	unsigned char* view = MapViewOfFile(
		map_handle, FILE_MAP_ALL_ACCESS, 0, 0, amount);

	assert(view != NULL, FOOD_VIEW_ERROR, -1);

	// writing neccessary information in view
	// 1st byte is the amount of food
	// 2nd byte is the current position
	unsigned char* msg = malloc(amount * sizeof(unsigned char));
	
	msg[0] = amount;
	msg[1] = 2;

	CopyMemory(view, msg, amount * sizeof(unsigned char));
	
	UnmapViewOfFile(view);	
	
	free(msg);
}

// creates hen process
void create_hen()
{
	wchar_t cmd_buffer[400];

	// constructing command line arguments
	wsprintf(cmd_buffer, HEN_CMD_PATTERN,
		HEN_NAME,
		FOOD_NAME,
		FOOD_MUTEX_NAME,
		FULL_SEMAPHORE_NAME,
		EMPTY_SEMAPHORE_NAME);
		
	STARTUPINFO hen_startup_info;
	PROCESS_INFORMATION hen_process_info;

	ZeroMemory(&hen_startup_info, sizeof(STARTUPINFO));
	ZeroMemory(&hen_process_info, sizeof(PROCESS_INFORMATION));

	hen_startup_info.cb = sizeof(STARTUPINFO);

	BOOL is_created = CreateProcess(
		NULL,
		cmd_buffer,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&hen_startup_info,
		&hen_process_info);

	assert(is_created == TRUE, HEN_ERROR, -1);

	wprintf(MSG_PATTERN, HEN_CREATION_MSG);

	CloseHandle(hen_process_info.hProcess);
	CloseHandle(hen_process_info.hThread);
}

// creates chicken processes
void create_chickens(int amount)
{
	wchar_t cmd_buffer[400];

	STARTUPINFO chicken_startup_info;
	PROCESS_INFORMATION chicken_process_info;
	
	BOOL is_chicken_created = FALSE;
	
	unsigned int startup_info_size = sizeof(STARTUPINFO);
	unsigned int process_info_size = sizeof(PROCESS_INFORMATION);

	// starting creation of chicken processes
	for (int i = 0; i < amount; i++)
	{
		ZeroMemory(&chicken_startup_info, startup_info_size);
		ZeroMemory(&chicken_process_info, process_info_size);

		chicken_startup_info.cb = startup_info_size;

		// constructing command line arguments
		wsprintf(cmd_buffer, CHICKEN_CMD_PATTERN,
			CHICKEN_NAME,
			FOOD_NAME,
			FOOD_MUTEX_NAME,
			FULL_SEMAPHORE_NAME,
			EMPTY_SEMAPHORE_NAME,
			i);

		is_chicken_created = CreateProcess(
			NULL,
			cmd_buffer,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&chicken_startup_info,
			&chicken_process_info);

		if (is_chicken_created == TRUE)
			wprintf(CHICKEN_CREATION_MSG, i);

		CloseHandle(chicken_process_info.hProcess);
		CloseHandle(chicken_process_info.hThread);
	}
}

// entry point for farmer
int wmain(int argc, wchar_t** argv)
{
	// processing and parsing command line arguments
	if (argc != 3)
		exit_with_message(INVALID_ARG_MSG, 1);

	int amount_of_chickens = _wtoi(argv[1]);
	int amount_of_food = _wtoi(argv[2]);

	// validating command line arguments
	if (amount_of_food > 253)
		exit_with_message(FOOD_AMOUNT_ERROR, -1);

	// creating synchronization objects
	create_sync_objects(amount_of_food);

	// creating map object
	create_food_mapping(amount_of_food);

	// creating hen process
	create_hen();

	// creating chicken processes
	create_chickens(amount_of_chickens);

	Sleep(SLEEP_TIME);

	return 0;
}