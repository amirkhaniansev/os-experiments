/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#ifndef HELPER_MODULE_H
#define HELPER_MODULE_H

#include <Windows.h>
#include <TlHelp32.h>

#define WIN_PERROR_FORMAT L"Error occured: %s"
#define WRONG_ARG_COUNT   L"One argument must be provided\n"

static HANDLE reader_handle = NULL;
static HANDLE writer_handle = NULL;
static HANDLE file_handle = NULL;
static DWORD  reader_pid = 0;
static DWORD  writer_pid = 0;

void win_perror();
void change_process_state(DWORD, void(*functor)(HANDLE));
BOOL copy_file(HANDLE, HANDLE);

#endif 