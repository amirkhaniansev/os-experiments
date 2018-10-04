/*
 * MIT License
 *
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 *
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

// defining macros for error messages
#define NO_ARG_ERROR "No arguments error"

// Unix perror equivalent for Windows
// for more information see https://www.github.com/amirkhaniansev/os-experiments/WinPerror
void win_perror() {
	wchar_t* err_msg = NULL;

	DWORD msg_size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(L"Last error is: %s", err_msg);

	// deallocating memory that OS allocated for storing message
	LocalFree(err_msg);
}

// Function for copying files
// This the modified version of CopyFile
// For more information see https://www.github.com/os-experiments/CopyFile
BOOL copy_file(HANDLE source_handle, HANDLE dest_handle) {
	if (source_handle == INVALID_HANDLE_VALUE || dest_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	// allocating buffer
	LPCVOID buffer = (unsigned char*)malloc(1024);

	// variables for read and write size information
	DWORD number_of_bytes_to_read = 256;
	DWORD number_of_bytes_to_write = 0;

	LPDWORD number_of_bytes_read = NULL;
	LPDWORD number_of_bytes_written = NULL;
	
	LPOVERLAPPED overlapped = NULL;

	do {
		// reading
		if (ReadFile(source_handle,
			buffer,
			number_of_bytes_to_read,
			&number_of_bytes_read,
			overlapped) == FALSE)
			break;

		// writing
		if (WriteFile(dest_handle,
			buffer,
			number_of_bytes_read,
			&number_of_bytes_written,
			overlapped) == FALSE)
			break;

	} while (number_of_bytes_read != 0);

	// deallocating memory
	free(buffer);

	return TRUE;
}

// gets input from stdin and copies to stdout
void std_in_out() {
	// getting handles
	HANDLE std_in_handle = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE std_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// starting endless input and output process
	while (1) {
		if (copy_file(std_in_handle, std_out_handle) == FALSE) {
			win_perror();
			break;
		}
	}
}

// main function, entry point for program
// TODO providing all functionality
int main(int argc, char** argv) {
	// if no arguments are provided
	// WinCat should get input from stdin and copy it to stdout
	if (argc == 1)
		std_in_out();
	return 0;
}