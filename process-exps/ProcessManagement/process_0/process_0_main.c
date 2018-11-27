#include <stdio.h>
#include <Windows.h>

int wmain(int argc, wchar_t** argv)
{
	// բացում ենք notepad-ը
	LPPROCESS_INFORMATION notepad_info = malloc(sizeof(PROCESS_INFORMATION));
	LPSTARTUPINFO notepad_start_info = malloc(sizeof(STARTUPINFO));
	
	ZeroMemory(notepad_start_info, sizeof(STARTUPINFO));
	ZeroMemory(notepad_info, sizeof(PROCESS_INFORMATION));

	notepad_start_info->cb = sizeof(STARTUPINFO);
	
	wchar_t notepad[] = L"notepad.exe";

	BOOL isNotepadCreated = CreateProcess(
		NULL, notepad, NULL, NULL, FALSE, 0 , NULL, NULL,
		notepad_start_info,
		notepad_info);

	if (isNotepadCreated == FALSE)
	{
		wprintf(L"Error : %d", GetLastError());
		exit(1);
	}

	wprintf(L"Notepad started\n");

	// բացում ենք process_2՝ 3-րդ պրոցեսը որը պետք է փակի notepad-ը
	LPPROCESS_INFORMATION p2_info = malloc(sizeof(PROCESS_INFORMATION));
	LPSTARTUPINFO p2_start_info = malloc(sizeof(STARTUPINFO));

	ZeroMemory(p2_start_info, sizeof(STARTUPINFO));
	ZeroMemory(p2_info, sizeof(PROCESS_INFORMATION));

	p2_start_info->cb = sizeof(STARTUPINFO);
	p2_start_info->lpTitle = L"P2";

	wchar_t cmd[200] = L"process_2.exe ";
	wchar_t hBuffer[200];
	wchar_t idBuffer[200];

	_itow_s(notepad_info->hProcess, hBuffer, 200, 10);
	_itow_s(GetCurrentProcessId(), idBuffer, 200, 10);

	wcscat_s(cmd, 100, hBuffer);
	wcscat_s(cmd, 100, L" \0");
	wcscat_s(cmd, 100, idBuffer);

	BOOL isP2Created = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL,
		p2_start_info, p2_info);

	if (isP2Created == FALSE)
	{
		wprintf(L"Error : %d", GetLastError());
		exit(1);
	}

	wprintf(L"process_2 started\n");

	WaitForSingleObject(notepad_info->hProcess, INFINITE);

	wprintf(L"process_0 exited\n");

	CloseHandle(notepad_info->hProcess);
	CloseHandle(notepad_info->hThread);
	CloseHandle(p2_info->hProcess);
	CloseHandle(p2_info->hThread);

	free(notepad_info);
	free(notepad_start_info);
	free(p2_info);
	free(p2_start_info);

	return 0;
}