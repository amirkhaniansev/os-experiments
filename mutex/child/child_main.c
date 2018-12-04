/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Full notice : https://github.com/amirkhaniansev/os-experiments/tree/master/LICENSE
 */

#include <stdio.h>
#include <Windows.h>

#define INVALID_ARG_MSG       L"Invalid amount of arguments.\n"
#define BUFFER_FILE_NAME      L"d:\\buffer.txt"
#define INVALID_HANDLE_MSG    L"Invalid file handle.\n"
#define MUTEX_NAME            L"f_mutex"
#define WAIT_ERROR	          L"Wait error.\n"
#define RECORD				  "record"
#define TEXT_PATTERN          "%5d : %s %d.\n"

HANDLE   m_handle     = NULL;
HANDLE   f_handle     = NULL;
DWORD    pid          = 0;
int      record_count = 0;

void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(msg);
	exit(exit_code);
}

void write_to_file()
{
	char buffer[400];
	sprintf_s(buffer, 400, TEXT_PATTERN, pid, RECORD, record_count++);

	char text[200];
	sprintf_s(text, 200, buffer);

	WriteFile(f_handle, text, strlen(buffer), NULL, NULL);
}

int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
		exit_with_message(INVALID_ARG_MSG, -1);

	pid = GetCurrentProcessId();
	m_handle = _wtoi(argv[1]);
	f_handle = _wtoi(argv[2]);

	DWORD wait_result = 0UL;

	while (1)
	{
		Sleep((rand() % 2) * 1000);

		wait_result = WaitForSingleObject(m_handle, INFINITE);

		if (wait_result != WAIT_OBJECT_0)
			exit_with_message(WAIT_ERROR, -1);

		write_to_file();

		ReleaseMutex(m_handle);
	}
}