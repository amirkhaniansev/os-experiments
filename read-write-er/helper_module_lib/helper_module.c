/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#include <stdio.h>
#include <wchar.h>

#include "helper_module.h"

/* win_perror - prints last error message of NT
 *
 * May be called after every system call.
 */
void win_perror() {
	LPWSTR err_msg = NULL;

	DWORD msg_size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(WIN_PERROR_FORMAT, err_msg);

	LocalFree(err_msg);
}


/**
 * copy_file - copies the file with source handle into the file with dest_handle
 * @source_handle - handle of source file
 * @dest_handle - handle of destination file
 * 
 * May be called to copy files.
 * Returns BOOL value which informs whether the copying operation is done correctly
 */
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

/**
 * change_process_state - changes process state 
 * @pid - process id
 * @functor - function which will be called for every thread of program.
 * 
 * Be careful,this function MUST be called only with the following values of functor:
 * SuspendThread or ResumeThread, otherwise there will be unexpected behavior(crash or something worse).
 */
 void change_process_state(DWORD pid, void(__stdcall* functor)(HANDLE)) {
	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	LPTHREADENTRY32 p_entry = (LPTHREADENTRY32)malloc(sizeof(THREADENTRY32));
	p_entry->dwSize = sizeof(THREADENTRY32);

	Thread32First(snapshot_handle, p_entry);

	do
	{
		if (p_entry->th32OwnerProcessID == pid)
		{
			HANDLE hThread = OpenThread(
				THREAD_ALL_ACCESS, 
				FALSE,
				p_entry-> th32ThreadID);

			functor(hThread);
			CloseHandle(hThread);
		}
	} while (Thread32Next(snapshot_handle, p_entry));

	free(p_entry);

	if (CloseHandle(snapshot_handle) == FALSE)
		win_perror();	
}
