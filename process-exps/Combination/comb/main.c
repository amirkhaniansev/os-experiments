#include <stdio.h>
#include <Windows.h>

struct factorial_info
{
	int n;
	int result;
} typedef f_info;

static f_info* init_f_info(int n)
{
	f_info* _f_info = malloc(sizeof(f_info));
	_f_info->n = n;	
	_f_info->result = 1;	
	return _f_info;
}

static void destroy_f_info(f_info* f_info)
{
	free(f_info);
}

static DWORD WINAPI calculate_factorial(void* factorial_info)
{
	f_info* factorialInfo = factorial_info;

	for (int i = 2; i < factorialInfo -> n + 1; i++)
		factorialInfo -> result *= i;
	
	return 0;
}

// պրոտոկոլը հետևյալն ա ՝ argv[1] = n, argv[2] = k
int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
	{
		wprintf(L"Invalid amount of args");
		exit(1);
	}
	 
	int n = _wtoi(argv[1]);
	int k = _wtoi(argv[2]);
	
	f_info* nf = init_f_info(n);
	f_info* kf = init_f_info(k);
	f_info* nkf = init_f_info(n - k);

	HANDLE* calculator_threads = malloc(3 * sizeof(HANDLE));
	DWORD* ids = malloc(3 * sizeof(DWORD));

	calculator_threads[0] = CreateThread(NULL, 0, calculate_factorial,
		nf, 0, &ids[0]);

	calculator_threads[1] = CreateThread(NULL, 0, calculate_factorial,
		kf, 0, &ids[1]);

	calculator_threads[2] = CreateThread(NULL, 0, calculate_factorial,
		nkf, 0, &ids[2]);

	WaitForMultipleObjects(3, calculator_threads, TRUE, INFINITE);

	int result = nf->result / (kf->result * nkf->result);

	wprintf(L"Answer : %d\n", result);

	destroy_f_info(nf);
	destroy_f_info(kf);
	destroy_f_info(nkf);

	CloseHandle(calculator_threads[0]);
	CloseHandle(calculator_threads[1]);
	CloseHandle(calculator_threads[2]);

	free(calculator_threads);
	free(ids);

	return 0;
}