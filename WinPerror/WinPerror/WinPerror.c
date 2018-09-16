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

// gets error message
wchar_t* get_error_message(DWORD err_code)
{
	// if error code is 0 , then there is no error
	if (err_code == 0)
		return L"No error detected";

	// defining message variable
	LPWSTR err_msg = NULL;

	// getting message corresponding to the given error code
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

// prints help information to console
void print_help_info()
{
	printf("Type\n");
	printf("-h for help\n");
	printf("-last for last error message of NT\n");
	printf("-[error code] for the error message corresponding to the error code\n");
	printf("-exit for closing the program\n");
}

// starts input for testing
// this function needs to be fixed
void start_input()
{
	print_help_info();

	char* buffer = malloc(256 * sizeof(buffer));
	const char* inv_in_msg = "Invalid input";

	while (1)
	{
		// be careful this part is not working properly
		int scan_result = scanf_s("%c",&buffer);

		if (scan_result == 1)
		{
			printf(inv_in_msg);

			continue;
		}
	  
		if (strcmp(buffer, "-h") == 0)
		{
			print_help_info();
		}
		else if (strcmp(buffer, "-last") == 0)
		{
			wprintf(get_last_error_message());
		}
		else if (strcmp(buffer, "-exit") == 0)
		{
			break;
		}
		else
		{
			int code = atoi(buffer);

			if (code == 0)
			{
				printf(inv_in_msg);	
				continue;
			}

			wprintf(get_error_message(code));
		}

		printf("\n");
	}

	free(buffer);
}

// entry point
int main()
{
	wprintf(get_error_message(4));
}