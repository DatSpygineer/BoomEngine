#pragma once

#include "puli_defs.h"

#ifdef _WIN32
	#include <Windows.h>

	typedef HMODULE dll_t;
#else
	#include <dlfcn.h>

	typedef void* dll_t;
#endif

dll_t pDllOpen(const char* path);
void* pDllSym(const dll_t* dll, const char* name);
void pDllClose(dll_t* handle);