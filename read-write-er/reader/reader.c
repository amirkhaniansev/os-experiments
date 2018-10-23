/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/blob/master/LICENSE
 */

#include <stdio.h>
#include "..\helper_module_lib\helper_module.h"

/**
 * ctrl_c_handler - handles CTRL + C event
 * @ctrl_type - type of CTRL event
 *
 * This function is called by operating system when one of CTRL events occur.
 * Returns BOOL value which informs the operating system
 * whether the event has been successfully handled.
 */
static BOOL WINAPI ctrl_c_handler(DWORD ctrl_type) {
	if (ctrl_type == CTRL_C_EVENT) {
		change_process_state(writer_pid, ResumeThread);
		change_process_state(reader_pid, SuspendThread);		
		return TRUE;
	}

	return FALSE;
}

/**
 * init - initializes the reader program
 *
 * May be called in the beginning of entry point.
 */
void init() {
	reader_handle = GetCurrentProcess();
	reader_pid = GetCurrentProcessId();

	DWORD size = sizeof(PROCESSENTRY32);
	LPPROCESSENTRY32 p_entry = (LPPROCESSENTRY32)malloc(size);
	p_entry->dwSize = size;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if(Process32First(snapshot,p_entry) == TRUE)
		while(Process32Next(snapshot,p_entry) == TRUE)
			if (wcscmp(p_entry->szExeFile, L"writer.exe") == 0) {
				writer_pid = p_entry->th32ProcessID;
				writer_handle = OpenProcess(
					PROCESS_ALL_ACCESS,
					FALSE,
					p_entry->th32ProcessID);
			}

	free(p_entry);

	if (CloseHandle(snapshot) == FALSE)
		win_perror();
}

/**
 * wmain - entry point for reader program
 * @argc - count of arguments
 * @argv - vector of arguments
 * @envp - environment variables
 *
 * Returns integer values which informs the operating system
 * whether the program has run correctly.
 */
int wmain(int argc, wchar_t** argv, wchar_t** envp) {
	if (argc == 1 || argc > 2) {
		wprintf(WRONG_ARG_COUNT);
		return -1;
	}

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_c_handler, TRUE) == FALSE) {
		win_perror();
		return -1;
	}

	init();

	file_handle = CreateFile(
		argv[1],
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);

	while(1) 
		if (copy_file(GetStdHandle(STD_INPUT_HANDLE), file_handle) == FALSE) {
			win_perror();

			return -1;
		}

	if (CloseHandle(file_handle) == FALSE)
		win_perror();

	return 0;
}