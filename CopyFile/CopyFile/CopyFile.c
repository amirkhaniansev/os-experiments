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
#include <string.h>

#include "CopyFile.h"

void win_perror() {
	wchar_t* err_msg = NULL;

	DWORD msg_size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(L"Last error is: %s", err_msg);

	LocalFree(err_msg);
}

BOOL copy_file(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists) {
	
	// if lengths are  then return FALSE
	if (wcslen(lpExistingFileName) == 0 || wcslen(lpNewFileName) == 0)
		return FALSE;

	// opening existing source file
	HANDLE source_handle = CreateFile(
		lpExistingFileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (source_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	// opening destination file
	HANDLE dest_handle = CreateFile(
		lpNewFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (dest_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	LPCVOID buffer = (unsigned char*)malloc(1024);

	DWORD number_of_bytes_to_read = 256, number_of_bytes_to_write = 0;
	LPDWORD number_of_bytes_read = NULL, number_of_bytes_written = NULL;
	LPOVERLAPPED overlapped = NULL;
	BOOL read_result, write_result;

	do {
		// reading
		read_result = ReadFile(source_handle,
			buffer,
			number_of_bytes_to_read,
			&number_of_bytes_read,
			overlapped);

		// writing
		write_result = WriteFile(dest_handle,
			buffer,
			number_of_bytes_read,
			&number_of_bytes_written,
			overlapped);
					
		// checking read-write results
		if (read_result == FALSE || write_result == FALSE)
			break;

	} while (number_of_bytes_read != 0);

	// deallocating memory
	free(buffer);

	return CloseHandle(source_handle) & CloseHandle(dest_handle);
}