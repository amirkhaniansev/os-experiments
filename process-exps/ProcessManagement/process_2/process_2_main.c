#include <stdio.h>
#include <Windows.h>

static HANDLE hRealNotepad = NULL;
static BOOL isClosed = FALSE;

static BOOL WINAPI ctrl_c_handler(DWORD ctrl_type) {
	if (ctrl_type == CTRL_C_EVENT) {
		TerminateProcess(hRealNotepad, 0);
		isClosed = TRUE;
		return TRUE;
	}

	return FALSE;
}

int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
	{
		wprintf(L"Invalid amount of args");
		exit(1);
	}

	SetConsoleCtrlHandler(ctrl_c_handler, TRUE);

	HANDLE hNotepad = _wtoi(argv[1]);
	DWORD p0_id = _wtoi(argv[2]);


	BOOL isDuplicated = DuplicateHandle(
		OpenProcess(PROCESS_DUP_HANDLE, FALSE, p0_id),
		hNotepad,
		GetCurrentProcess(),
		&hRealNotepad,
		PROCESS_TERMINATE,
		FALSE,
		0);

	if (isDuplicated == FALSE)
	{
		wprintf(L"Unable to duplicate handle");
		exit(1);
	}

	while (1)
	{
		Sleep(5);

		if (isClosed == TRUE)
		{
			wprintf(L"Notepad closed");
			break;
		}
	}

	CloseHandle(hRealNotepad);
}