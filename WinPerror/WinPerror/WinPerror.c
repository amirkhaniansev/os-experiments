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

// defining macros for having colourful output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// defining macros for help info
#define TYPE_MSG "Type\n"
#define HELP_MSG "-h for help\n"
#define LAST_MSG "-last for last error message of NT\n"
#define CODE_MSG "-[error code] for the error message corresponding to the error code\n"
#define EXIT_MSG "-exit for closing the program\n"

// defining buffer size
#define BUFF_SIZE 0x100

// gets error message
wchar_t* get_error_message(DWORD err_code)
{
	// if error code is 0 , then there is no error
	if (err_code == 0)
		return L"No error detected";

	// defining message variable
	LPWSTR err_msg = NULL;

	// getting message corresponding to the given error code
	/* FORMAT_MESSAGE_ALLOCATE_BUFFER :
	 * The function allocates a buffer large enough to hold the formatted message, 
	 * and places a pointer to the allocated buffer at the address specified by lpBuffer.
	 *
	 * FORMAT_MESSAGE_FROM_SYSTEM :
     * The function should search the system message-table resource(s) 
	 * for the requested message.
	 *
	 * FORMAT_MESSAGE_IGNORE_INSERTS :
	 * Insert sequences in the message definition are to be ignored and 
	 * passed through to the output buffer unchanged.
	 */
	// for more info please see https://docs.microsoft.com (MSDN)
	DWORD msg_size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err_code, 0, &err_msg, 0, NULL);

	// if there is no error message for that error code return no error
	if (msg_size == 0)
		return L"No Error";

	// othwerwise return error code
	return (wchar_t*)err_msg;
}

// gets the message of NT last error
wchar_t* get_last_error_message()
{
	return get_error_message(GetLastError());
}

// prints the given message in the specified color
void print_specific_message(const char* color,const char* message)
{
	printf(color);
	printf(message);
	printf(ANSI_COLOR_RESET);
}

// prints help information to console
void print_help_info()
{
	printf("%s%s%s%s%s",TYPE_MSG, HELP_MSG, LAST_MSG, CODE_MSG, EXIT_MSG);
}

// initializes input process
// ends when user enters the appropriate command: -exit.
void init_input_process()
{
	// printing help info
	print_help_info();

	// allocating buffer
	char* buffer = (char*)malloc(BUFF_SIZE * sizeof(char));

	// const variable for invalid input message
	const char* inv_in_msg = "Invalid input";

	// starting endless loop for input
	// this will in end in the case user enters -exit
	while (1)
	{
		printf("\n");

		// scanning stdin stream
		if (!scanf_s("%s", buffer, BUFF_SIZE) || *buffer != '-')
		{
			print_specific_message(ANSI_COLOR_RED, inv_in_msg); continue;
		}
	  
		// processing input
		if (!strcmp(buffer, "-h"))
			print_help_info();		
		else if (!strcmp(buffer, "-last"))
			wprintf(get_last_error_message());
		else if (!strcmp(buffer, "-exit"))
			break;
		else
		{
			// parsing string to int
			int code = atoi(buffer + 1);

			if (!code)
			{
				print_specific_message(ANSI_COLOR_RED, inv_in_msg); continue;
			}

			// printing NT error message corresponding to the error message ID entered by the user
			wprintf(get_error_message(code));
		}
	}

	// deallocating memory
	// i.e giving memory back to OS by calling OS allocator
	free(buffer);
}

// entry point
int main()
{
	init_input_process();
	return 0;
}