/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#ifndef HELPER_MODULE_H
#define HELPER_MODULE_H

#include <Windows.h>
#include <TlHelp32.h>

/* macros */
#define WIN_PERROR_FORMAT L"Error occured: %s"
#define WRONG_ARG_COUNT   L"One argument must be provided\n"

/* global variables */
static HANDLE reader_handle = NULL;
static HANDLE writer_handle = NULL;
static HANDLE file_handle   = NULL;
static DWORD  reader_pid    = 0;
static DWORD  writer_pid    = 0;
static DWORD  file_begin    = FILE_BEGIN;

/* function signatures */
void win_perror();
void change_process_state(DWORD, void(__stdcall *functor)(HANDLE));
BOOL copy_file(HANDLE, HANDLE);

#endif 