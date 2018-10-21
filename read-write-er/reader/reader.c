/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#include <stdio.h>
#include "..\helper_module_lib\helper_module.h"

static BOOL WINAPI ctrl_c_handler(DWORD ctrl_type) {
	return TRUE;
}

int wmain(int argc, wchar_t** argv, wchar_t** envp) {
	if (argc == 1 || argc > 2) {
		wprintf(WRONG_ARG_COUNT);
		return -1;
	}

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_c_handler, TRUE) == FALSE) {
		win_perror();
		return -1;
	}

	file_handle = CreateFile(
		argv[1],
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	while(1) 
		if (copy_file(GetStdHandle(STD_INPUT_HANDLE), file_handle) == FALSE) {
			win_perror();

			if (CloseHandle(file_handle) == FALSE)
				win_perror();

			return -1;
		}

	return 0;
}