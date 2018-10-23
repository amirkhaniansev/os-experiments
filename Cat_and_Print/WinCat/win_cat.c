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
#include <string.h>

#include <Windows.h>

// global variable for copying indicator
BOOL TO_CONTINUE = TRUE;

// Helper methods

// Unix perror equivalent for Windows
// for more information see https://www.github.com/amirkhaniansev/os-experiments/WinPerror
static void win_perror() {
	wchar_t* err_msg = NULL;

	// getting error message with the the given code from GetLastError()
	DWORD msg_size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(L"Error occured: %s", err_msg);

	// deallocating memory that OS allocated for storing message
	LocalFree(err_msg);
}

// CTRL+C event handler
static BOOL ctrl_c_handler(DWORD ctrltype) {
	// if Ctrl + C signal is receieved then process
	// making TO_CONTINUE false which will inform copy_file function to stop copying
	if (ctrltype == CTRL_C_EVENT) {
		TO_CONTINUE = FALSE;
		// here returning TRUE to tell the NT that Ctrl + C is handled
		return TRUE;
	}
	return FALSE;
}

// Function for copying files
// This the modified version of CopyFile
// For more information see https://www.github.com/amirkhaniansev/os-experiments/CopyFile
static BOOL copy_file(HANDLE source_handle, HANDLE dest_handle) {
	// if any of handles is invalid then return FALSE indicating error
	if (source_handle == INVALID_HANDLE_VALUE || dest_handle == INVALID_HANDLE_VALUE)
		return FALSE;

	TO_CONTINUE = TRUE;

	// allocating buffer
	LPCVOID buffer = (unsigned char*)malloc(1024);

	// variables for read and write size information
	DWORD number_of_bytes_to_read = 256;
	DWORD number_of_bytes_to_write = 0;

	LPDWORD number_of_bytes_read = NULL;
	LPDWORD number_of_bytes_written = NULL;
	
	LPOVERLAPPED overlapped = NULL;

	do {
		// if TO_CONTINUE indicator is false stop copying process
		if (TO_CONTINUE == FALSE) {
			wprintf(L"\nCopying is interrupted...\n");
			break;
		}

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
static void std_in_out() {
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

// copies files to output file if it is not null and to stdout otherwise
static void copy_to_output_file(int amount, wchar_t** file_names, HANDLE dest_handle) {
	HANDLE source_handle;
	int counter = 0;

	// starting copying
	while (counter < amount) {
		// getting source handle
		source_handle = CreateFile(
			file_names[counter],
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		wprintf(L"\nCopying %s...\n\n", file_names[counter++]);

		if (copy_file(source_handle, dest_handle) == FALSE)
			win_perror();

		if (CloseHandle(source_handle) == FALSE)
			win_perror();
	}
}

// main function, entry point for program
// TODO: handle Ctrl + C signal
int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
	// if no arguments are provided
	// WinCat should get input from stdin and copy it to stdout
	if (argc == 1)
		std_in_out();
	// otherwise it should process the arguments
	// 1st case: File1 File2 ... ] FileN : should write File1-FileN-1 in FileN
	// 2nd case: File1 File2 ... FileN : should write File1-FileN in stdout
	else if (argc > 1) {
		HANDLE dest_handle = 0;
		int file_amount = 0;
		wchar_t* last_arg = argv[argc - 2];

		// determining destination handler
		if (strcmp(last_arg,"]") == 0) {
			dest_handle = CreateFile(
				argv[argc - 1],
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			file_amount = argc - 3;
		}
		else {
			dest_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			file_amount = argc - 1;
		}		

		// registering CTRL+C handler to process handler routine
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_c_handler, TRUE) == FALSE)
			return 0;

		// starting copying
		copy_to_output_file(file_amount, argv + 1, dest_handle);
	}

	return 0;
}
