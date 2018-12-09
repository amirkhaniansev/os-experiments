#include <stdio.h>
#include <string.h>
#include <Windows.h>

#define THREAD_COUNT        3
#define BUFFER_SIZE         4000
#define TEXT_SIZE           40000000
#define SPACE               ' '
#define TAB			        '\t'
#define NEW_LINE            '\n'
#define NEW_LINE_UNICODE    L"\n"
#define END                 '\0'
#define DOT                 '.'
#define SEMICOLON           ';'
#define COMMA			    ','
#define COLON               ':'
#define INPUT_FORMAT        "%s"
#define WORDS_COUNT         L"Words Count : %d\n"
#define BYTES_COUNT		    L"Bytes Count : %d\n"
#define TEXT_FORMAT         "%s\n"
#define INVALID_ARGS        L"Invalid arguments.\n"
#define EVENT_ERROR         L"Unable to create event.\n"
#define EVENT_SET_ERROR     L"Unable to set event.\n"
#define EVENT_RESET_ERROR   L"Unable to reset event.\n"
#define CTRL_C_EVENT_ERROR  L"Unable to set CTRL_C_EVENT handler.\n"

HANDLE   event = NULL;
HANDLE*  threads = NULL;
char*    buffer = NULL;
char*    text = NULL;
int*     ids = NULL;
int      buffer_length = 0;
int      text_length = 0;

void exit_with_message(wchar_t* msg, int exit_code)
{
	wprintf(msg);
	exit(exit_code);
}

void init()
{
	threads = malloc(sizeof(HANDLE) * THREAD_COUNT);
	buffer = malloc(sizeof(char) * BUFFER_SIZE);
	text = malloc(sizeof(char) * TEXT_SIZE);
	ids = malloc(sizeof(int) * THREAD_COUNT);
}

void finalize()
{
	for (int i = 0; i < THREAD_COUNT; i++)
		CloseHandle(threads[i]);

	CloseHandle(event);

	free(threads);
	free(ids);
	free(buffer);
	free(text);
}

void create_thread_with_routine(int number, LPTHREAD_START_ROUTINE routine)
{
	threads[number] = CreateThread(
		NULL, 0, routine, NULL, 0, &ids[number]);
}

void start_input()
{
	while (1)
	{
		gets(buffer);

		buffer_length = strlen(buffer);	

		sprintf_s(text + text_length, TEXT_SIZE, buffer);

		if (SetEvent(event) == FALSE)
			exit_with_message(EVENT_SET_ERROR, -1);

		if (ResetEvent(event) == FALSE)
			exit_with_message(EVENT_RESET_ERROR, -1);

		text_length = strlen(text);
	}
}

BOOL WINAPI ctrl_c_handle(DWORD ctrl_type)
{
	if (ctrl_type != CTRL_C_EVENT)
		return FALSE;

	for (int i = 0; i < THREAD_COUNT; i++)
		TerminateThread(threads[i], 0);

	return TRUE;
}

DWORD WINAPI count_words(void* param)
{
	int i = 0U;
	unsigned char word = 0;
	unsigned int word_count = 0U;	

	while (1) 
	{
		WaitForSingleObject(event, INFINITE);

		i = 0;
		word = 0;
		word_count = 0U;

		do 
		{
			switch (buffer[i]) 
			{
				case SPACE:
				case TAB:
				case NEW_LINE:
				case END:
				case COLON:
				case SEMICOLON:
				case DOT:
				case COMMA:
					if (word)
					{
						word = 0;
						word_count++;
					}
					break;
				default: word = 1;
			}
		} while (i++ < buffer_length);	

		wprintf(WORDS_COUNT, word_count);
	}

	return 0;
}

DWORD WINAPI count_bytes(void* param)
{
	while (1)
	{
		WaitForSingleObject(event, INFINITE);

		unsigned int bytes_count = buffer_length * sizeof(char);

		wprintf(BYTES_COUNT, bytes_count);
	}

	return 0;
}

DWORD WINAPI make_capital_and_print(void* param)
{
	while (1)
	{
		WaitForSingleObject(event, INFINITE);

		char* temp_buff = malloc(sizeof(char) * buffer_length);

		for (int i = 0; i < buffer_length; i++)
		{
			if (buffer[i] > 97 && buffer[i] < 123)
				temp_buff[i] = buffer[i] - 32;
			else temp_buff[i] = buffer[i];
		}

		free(temp_buff);
	}

	return 0;
}

int wmain(int argc, wchar_t** argv)
{
	if (argc != 1)
		exit_with_message(INVALID_ARGS, -1);

	if (SetConsoleCtrlHandler(ctrl_c_handle, TRUE) == FALSE)
		exit_with_message(CTRL_C_EVENT_ERROR, -1);

	init();

	event = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (event == NULL)
		exit_with_message(EVENT_ERROR, -1);

	create_thread_with_routine(0, count_words);
	create_thread_with_routine(1, count_bytes);
	create_thread_with_routine(2, make_capital_and_print);

	start_input();

	WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);

	finalize();

	return 0;
}