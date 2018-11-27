#include <stdio.h>
#include <math.h>
#include <Windows.h>

struct interval_info 
{
	int inf;
	int sup;
} typedef interval;

static interval* init_interval(int inf, int sup)
{
	interval* _interval = malloc(sizeof(interval));
	_interval->inf = inf;
	_interval->sup = sup;
	return _interval;
}

static void destroy_interval(interval* _interval)
{
	free(_interval);
}

static CRITICAL_SECTION critical_section;
static int* prime_numbers = NULL;
static int  enumerator = 0;


static BOOL is_prime(int n)
{
	if (n < 2)
		return FALSE;

	if (n == 2 || n == 3)
		return TRUE;

	for (int i = 2; i <= sqrt(n); i++)
		if (n % i == 0)
			return FALSE;

	return TRUE;
}

static void print(const wchar_t* msg)
{
	wprintf(msg);

	for (int i = 0; i < enumerator; i++)
		wprintf(L"%d ", prime_numbers[i]);

	wprintf(L"\n");
}

static int comparator_func(const void* i, const void* j)
{
	return (*(int*)i - *(int*)j);
}

static DWORD WINAPI populate_prime_numbers(void* _int)
{
	interval* _interval = _int;

	for (int i = _interval->inf; i < _interval->sup; i++)
	{
		if (is_prime(i) == TRUE)
		{
			EnterCriticalSection(&critical_section);
			prime_numbers[enumerator++] = i;
			LeaveCriticalSection(&critical_section);
		}
	}

	return 0;
}

// պրոտոկոլը հետևյալն ա ՝ argv[1] = ստորին սահման, argv[2] = վերին սահման
int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
	{
		wprintf(L"Invalid amount of args\n");
		exit(1);
	}

	InitializeCriticalSection(&critical_section);
	
	int inf = _wtoi(argv[1]);
	int sup = _wtoi(argv[2]);

	// օգտվում ենք այն փաստից որ մինչև n-ը պարզ թվերի քանակը ~ n/logn
	// ավելի լավ գնահատական է ինտեգրալ 2-ից n dt/logt
	// 100 գումարում ենք նավսյակի
	int length = (sup / log(sup)) - (inf / log(inf)) + 100;

	prime_numbers = malloc(length * sizeof(int));

	interval* int0 = init_interval(inf, (sup - inf) / 2);
	interval* int1 = init_interval((sup - inf) / 2, sup + 1);

	HANDLE* threadHandles = malloc(2 * sizeof(HANDLE));
	DWORD* ids = malloc(2 * sizeof(DWORD));

	threadHandles[0] = CreateThread(NULL, 0, populate_prime_numbers,
		int0, 0, &ids[0]);

	threadHandles[1] = CreateThread(NULL, 0, populate_prime_numbers,
		int1, 0, &ids[1]);

	WaitForMultipleObjects(2, threadHandles, TRUE, INFINITE);

	destroy_interval(int0);
	destroy_interval(int1);

	CloseHandle(threadHandles[0]);
	CloseHandle(threadHandles[1]);

	free(threadHandles);
	free(ids);

	DeleteCriticalSection(&critical_section);
	
	print(L"Befor sorting : ");

	qsort(prime_numbers, enumerator, sizeof(int), comparator_func);
	
	print(L"After sorting : ");

	free(prime_numbers);

	return 0;
}