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
#include <wchar.h>

#include <Windows.h>

 /* macros for helping to do output */
#define WIN_PERROR_FORMAT L"Error occured: %s"
#define IS_RO		      L"Is Readonly"
#define F_TIME            L"File Creation Time"
#define F_SIZE			  L"File Size in bytes"
#define IS_DIR			  L"Is Directory"
#define F_NAME            L"File Name"
#define TABLE_FORMAT	  L"%50s%15s%36s%44s%15s\n" 
#define IS_RO_FORMAT	  L"%15s" 
#define F_TIME_FORMAT	  L"%15d.%02d.%02d %02d:%02d:%02d:%03d"
#define F_SIZE_FORMAT	  L"     %016x H | %016x L"
#define IS_DIR_FORMAT	  L"%15s\n"
#define F_NAME_FORMAT	  L"%47.40s..."
#define YES				  L"YES"
#define NO                L"No"
#define TOO_MANY_ARG_MSG  L"Too many arguments are provided"

/**
 * win_perror - prints last error message from NT
 *
 * May be called after system calls.
 */
static void win_perror() {
	LPWSTR err_msg = NULL;

	DWORD msg_size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, &err_msg, 0, NULL);

	wprintf(WIN_PERROR_FORMAT, err_msg);

	LocalFree(err_msg);
}

/**
 * concat_wild_card - concates wild card to directory
 * @dir - directory
 * @wild_card - wild card
 * @len - length of the concatenation result
 *
 * May be called with valid NOT NULL argumenst
 */
static wchar_t* concat_wild_card(wchar_t* dir, const wchar_t* wild_card) {
	size_t dir_len = wcslen(dir);
	size_t wild_card_len = wcslen(wild_card);
	unsigned int i = 0, j = 0;

	wchar_t* result = (wchar_t*)malloc((dir_len + wild_card_len + 1) * sizeof(wchar_t));

	for (i = 0; i < dir_len; i++)
		result[i] = dir[i];

	for (j = 0; j < wild_card_len; j++)
		result[i++] = wild_card[j];

	result[i] = '\0';

	return result;
}

/**
 * print_data - prints data to std out
 * @p_data - pointer to data about file
 *
 * May be called with pointer to valid data.
 */
static void print_data(LPWIN32_FIND_DATAW p_data) {
	if (p_data == NULL)
		return;

	/* printing file name */
	wprintf(F_NAME_FORMAT, p_data->cFileName);

	/* printing if file is readonly */
	wchar_t* yes_or_no = NULL;

	if (p_data->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
		yes_or_no = YES;
	else yes_or_no = NO;

	wprintf(IS_RO_FORMAT, yes_or_no);

	/* printing file creation time */
	LPSYSTEMTIME p_time = (LPSYSTEMTIME)malloc(sizeof(SYSTEMTIME));

	if (FileTimeToSystemTime(&p_data->ftCreationTime, p_time) == TRUE)
		wprintf(F_TIME_FORMAT,
			p_time->wDay, p_time->wMonth, p_time->wYear,
			p_time->wHour, p_time->wMinute, p_time->wSecond, p_time->wMilliseconds);

	free(p_time);

	/* printing file size */
	wprintf(F_SIZE_FORMAT, p_data->nFileSizeHigh, p_data->nFileSizeLow);

	/* printing file type : directory or file */
	if (p_data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		yes_or_no = YES;
	else yes_or_no = NO;

	wprintf(IS_DIR_FORMAT, yes_or_no);
}

/**
 * print_files_info - prints information about all files in the given directory
 * @dir - directory
 *
 * May be called with the valid directory.
 */
static void print_files_info(LPCWSTR dir) {
	if (dir == NULL)
		return;

	/* finding first file */
	LPWIN32_FIND_DATAW file_data = (LPWIN32_FIND_DATAW)malloc(sizeof(WIN32_FIND_DATAW));
	HANDLE find_handle = FindFirstFileW(dir, file_data);

	if (find_handle == INVALID_HANDLE_VALUE) {
		win_perror();
		return;
	}

	/* printing table title */
	wprintf(TABLE_FORMAT,
		F_NAME, IS_RO, F_TIME, F_SIZE, IS_DIR);

	/* starting printing file information */
	do {
		if (file_data != NULL)
			print_data(file_data);
	} while (FindNextFileW(find_handle, file_data) != FALSE);

	free(file_data);

	if (FindClose(find_handle) == FALSE)
		win_perror();
}

/**
* wmain - entry point for program
* @argc - count of arguments
* @argv - vector of arguments
* @envp - environment variables
*
* Returns integer value indicating error code for Operating system.
*/
int wmain(int argc, wchar_t** argv, wchar_t** envp) {
	if (argc > 2) {
		wprintf(TOO_MANY_ARG_MSG);
		return -1;
	}

	wchar_t* dir = NULL;
	BOOL allocated = FALSE;

	if (argc == 1) {
		DWORD buffer_size = GetCurrentDirectoryW(0, NULL);

		dir = (wchar_t*)malloc((buffer_size + 1) * sizeof(wchar_t));
		allocated = TRUE;

		buffer_size = GetCurrentDirectoryW(buffer_size + 1, dir);
	}
	else
		dir = argv[1];

	wchar_t* dir_with_wc = concat_wild_card(dir, L"\\*.*");

	print_files_info(dir_with_wc);

	if (allocated == TRUE)
		free(dir);

	free(dir_with_wc);

	return 0;
}