/*
 * MIT License
 *
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>

#include <Windows.h>

/* static global varibable for indicating copying consecutive process */
static BOOL TO_CONTINUE = TRUE;

/**
 * ctrl_c_handler - handles CTRL + C signal from OS
 * @ctrl_type: CTRL event type
 *
 * Returns BOOL indicating if the CTRL + C event is handled
 * This function is called by system in a separate thread when 
 * CTRL signal is recieved from keyboard.
 */
static BOOL WINAPI ctrl_c_handler(DWORD ctrl_type) {
	if (ctrl_type == CTRL_C_EVENT) {
		TO_CONTINUE = FALSE;

		return TRUE;
	}

	return FALSE;
}

/**
 * win_perror - prints last error message from NT
 *
 * May be called after system calls.
 */
 static void win_perror() {
	wchar_t* err_msg = NULL;

	DWORD msg_size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(L"Error occured: %s", err_msg);

	LocalFree(err_msg);
}

/**
 * copy_files - copies the given source files into destination file, closes all files before returning
 * @amount: amount of source handles
 * @source_handles: pointer to source handles
 * @dest_handle: destination file handle
 *
 * Returns BOOL indicating if the copying is done successfully.
 * May be called to copy multiple files into one file.
 */
static BOOL copy_files(int amount, HANDLE* source_handles, HANDLE dest_handle) {
	if (source_handles == NULL ||
		dest_handle == NULL ||
		dest_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	LPCVOID buffer = (unsigned char*)malloc(1024);
	
	DWORD number_of_bytes_to_read = 256, number_of_bytes_to_write = 0;
	
	LPDWORD number_of_bytes_read = NULL, number_of_bytes_written = NULL;

	LPOVERLAPPED overlapped = NULL;

	const wchar_t* copy_msg = L"Copying File";

	for (int counter = 0; counter < amount; counter++) {
		// here we have synchronization problem which needs to be fixed
		TO_CONTINUE = TRUE;

		if (source_handles[counter] == INVALID_HANDLE_VALUE)
			continue;
		
		wprintf(L"%s %d", copy_msg, counter);

		do {
			if (TO_CONTINUE == FALSE) {
				wprintf(L"\nCopying is interrupted...\n");
				break;
			}

			if (ReadFile(source_handles[counter],
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

		if (CloseHandle(source_handles[counter] == FALSE))
			win_perror();
	}

	if (CloseHandle(dest_handle) == FALSE)
		win_perror();

	free(buffer);

	return TRUE;
}

/**
 * wmain - entry point for program
 * @argc - count of arguments
 * @argv - vector of arguments
 * @envp - environment variables
 *
 * Returns integer value indicating error code for Operating system.
 */
int wmain(int argc, wchar_t** argv, wchar_t** envp) {
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_c_handler, TRUE) == FALSE)
		win_perror();

	const wchar_t* no_arg_msg = L"Arguments must be provided",
		copy_err_msg = L"Something happened while copying";

	if (argc == 1)
		wprintf(no_arg_msg);
	else {
		HANDLE* source_handles = (HANDLE*)malloc((argc - 1) * sizeof(HANDLE));
		
		for (int counter = 1; counter < argc; counter++)
			source_handles[counter - 1] = CreateFileW(
				argv[counter],
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

		if (copy_files(argc - 1, source_handles, GetStdHandle(STD_OUTPUT_HANDLE)) == FALSE)
			wprintf(copy_err_msg);

		free(source_handles);
	}

	return 0;
}