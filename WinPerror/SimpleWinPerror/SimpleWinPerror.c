/*
 * MIT License
 *
 * Copyright (c) Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 *
 */

#include <stdio.h>
#include <Windows.h>

void win_perror()
{
	wchar_t* err_msg = NULL;

	DWORD msg_size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(L"Last error is: %s", err_msg);

	// this part needs to be fixed
	free(err_msg);
}

int main()
{
	win_perror();

	return 0;
}