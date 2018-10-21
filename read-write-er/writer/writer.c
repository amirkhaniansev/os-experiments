/*
 * MIT License
 * Copyright (c) 2018 Sevak Amirkhanian <amirkhanyan.sevak@gmail.com>
 * For full notice please see https://github.com/amirkhaniansev/os-experiments/LICENSE
 */

#include <stdio.h>
#include "../helper_module_lib/helper_module.h"

static BOOL WINAPI ctrl_c_handler(DWORD ctrl_type) {
	return TRUE;
}

int wmain(int argc, wchar_t** argv, wchar_t** envp) {
	while (1) {}
}
