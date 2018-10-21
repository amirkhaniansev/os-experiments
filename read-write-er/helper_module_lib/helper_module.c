/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#include <stdio.h>
#include <wchar.h>

#include "helper_module.h"

void win_perror() {
	LPWSTR err_msg = NULL;

	DWORD msg_size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, err_msg, 0, NULL);

	wprintf(WIN_PERROR_FORMAT, err_msg);

	LocalFree(err_msg);
}


BOOL copy_file(HANDLE source_handle, HANDLE dest_handle) {
	if (source_handle == INVALID_HANDLE_VALUE || dest_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	LPCVOID buffer = (unsigned char*)malloc(1024);

	DWORD number_of_bytes_to_read = 256;
	DWORD number_of_bytes_to_write = 0;

	LPDWORD number_of_bytes_read = NULL;
	LPDWORD number_of_bytes_written = NULL;

	LPOVERLAPPED overlapped = NULL;

	do {
		if (ReadFile(source_handle,
			buffer,
			number_of_bytes_to_read,
			&number_of_bytes_read,
			overlapped) == FALSE)
			break;

		if (WriteFile(dest_handle,
			buffer,
			number_of_bytes_read,
			&number_of_bytes_written,
			overlapped) == FALSE)
			break;

	} while (number_of_bytes_read != 0);

	free(buffer);

	return TRUE;
}

void change_process_state(DWORD pid, void(__stdcall* functor)(HANDLE)) {
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	LPTHREADENTRY32 threadEntry = (LPTHREADENTRY32)malloc(sizeof(THREADENTRY32));
	threadEntry->dwSize = sizeof(THREADENTRY32);

	Thread32First(hThreadSnapshot, threadEntry);

	do
	{
		if (threadEntry->th32OwnerProcessID == pid)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
				threadEntry -> th32ThreadID);

			functor(hThread);
			CloseHandle(hThread);
		}
	} while (Thread32Next(hThreadSnapshot, threadEntry));

	free(threadEntry);

	if (CloseHandle(hThreadSnapshot) == FALSE)
		win_perror();	
}
