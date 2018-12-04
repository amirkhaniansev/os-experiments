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
#define SPACE_STRING	      L" \0"
#define CHILD_NAME            L"child.exe "
#define CHILD_STARTED_PATTERN L"Child %d started.\n"
#define CHILD_ERROR           L"Unable to create child.\n"

void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(msg);
	exit(exit_code);
}

void init_cmd_args(wchar_t* cmd_buffer, wchar_t* mutex_handle_buffer, wchar_t* file_handle_buffer,
	HANDLE mutex_handle, HANDLE file_handle)
{
	_itow_s(mutex_handle, mutex_handle_buffer, 200, 10);
	_itow_s(file_handle, file_handle_buffer, 200, 10);

	wcscat_s(cmd_buffer, 100, mutex_handle_buffer);
	wcscat_s(cmd_buffer, 100, SPACE_STRING);
	wcscat_s(cmd_buffer, 100, file_handle_buffer);
}

LPSECURITY_ATTRIBUTES init_security_attributes()
{
	unsigned int size = sizeof(SECURITY_ATTRIBUTES);

	LPSECURITY_ATTRIBUTES sec_attributes = malloc(size);
	sec_attributes->nLength = size;
	sec_attributes->bInheritHandle = TRUE;
	sec_attributes->lpSecurityDescriptor = NULL;
	
	return sec_attributes;
}

// ծրագրի մուտքի կետ
int wmain(int argc, wchar_t** argv)
{
	if (argc != 1)
		exit_with_message(INVALID_ARG_MSG, -1);

	LPSECURITY_ATTRIBUTES sec_attributes = init_security_attributes();

	HANDLE file_handle = CreateFile(
		BUFFER_FILE_NAME,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		sec_attributes,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (file_handle == INVALID_HANDLE_VALUE)
		exit_with_message(INVALID_HANDLE_MSG, -1);

	HANDLE mutex_handle = CreateMutex(
		sec_attributes,
		FALSE,
		MUTEX_NAME);

	if (mutex_handle == INVALID_HANDLE_VALUE)
		exit_with_message(INVALID_HANDLE_MSG, -1);

	wchar_t cmd_buffers[2][200] = { CHILD_NAME, CHILD_NAME };
	wchar_t m_buffer[200], f_buffer[200];
	LPSTARTUPINFO startups[2];
	LPPROCESS_INFORMATION infos[2];
	HANDLE p_handles[2];
	unsigned int startup_info_size = sizeof(STARTUPINFO);
	unsigned int process_info_size = sizeof(PROCESS_INFORMATION);

	for (int i = 0; i < 2; i++)
	{
		ZeroMemory(m_buffer, 200);
		ZeroMemory(f_buffer, 200);
		
		startups[i] = malloc(startup_info_size);
		infos[i] = malloc(process_info_size);

		ZeroMemory(startups[i], startup_info_size);
		ZeroMemory(infos[i], process_info_size);

		startups[i]->cb = startup_info_size;

		_itow_s(mutex_handle, m_buffer, 200, 10);
		_itow_s(file_handle, f_buffer, 200, 10);

		wcscat_s(cmd_buffers[i], 100, m_buffer);
		wcscat_s(cmd_buffers[i], 100, SPACE_STRING);
		wcscat_s(cmd_buffers[i], 100, f_buffer);

		BOOL is_created = CreateProcess(
			NULL,
			cmd_buffers[i],
			sec_attributes,
			sec_attributes,
			TRUE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			startups[i],
			infos[i]);

		p_handles[i] = infos[i]->hProcess;

		if (is_created == FALSE)
			exit_with_message(CHILD_ERROR, -1);

		wprintf(CHILD_STARTED_PATTERN, i);
	}

	WaitForMultipleObjects(2, p_handles, TRUE, INFINITE);

	for (int i = 0; i < 2; i++)
	{
		CloseHandle(infos[i]->hProcess);
		CloseHandle(infos[i]->hThread);

		free(infos[i]);
		free(startups[i]);
	}

	CloseHandle(mutex_handle);
	CloseHandle(file_handle);

	return 0;
}