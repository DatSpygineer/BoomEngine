#pragma once

#ifdef __cplusplus
	#include <cstdio>
	#include <cstdlib>
	#include <cstdint>
	#include <cstdarg>
	#include <cstring>
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <stdarg.h>
	#include <stdbool.h>
	#include <string.h>
#endif

#ifdef __cplusplus
	#define BOOM_HEADER_START extern "C" {
#else
	#define BOOM_HEADER_START
#endif

#ifdef __cplusplus
	#define BOOM_HEADER_END }
#else
	#define BOOM_HEADER_END
#endif

BOOM_HEADER_START

#ifdef _WIN32
	#include <Windows.h>

	#if SIZE_MAX == UINT32_MAX
		#define SSIZE_MAX INT32_MAX
		typedef int32_t ssize_t;
	#else
		#define SSIZE_MAX INT64_MAX
		typedef int64_t ssize_t;
	#endif
#else
	#include <unistd.h>
#endif

#ifdef BOOM_DLL_EXPORT
	#ifdef _WIN32
		#define BOOM_API __declspec(dllexport)
	#else
		#define BOOM_API __attribute__((visibility("default")))
	#endif
#else
	#ifdef _WIN32
		#define BOOM_API __declspec(dllimport)
	#else
		#define BOOM_API
	#endif
#endif

#define CONCAT_(__a, __b) __a##__b
#define CONCAT(__a, __b) CONCAT_(__a, __b)
#define UNIQUE_VARNAME(__prefix) CONCAT(__prefix, __LINE__)

#define DEFER(__ptr, __freeFn) \
for (bool UNIQUE_VARNAME(__defer_i_) = 0; UNIQUE_VARNAME(__defer_i_) == 0; (UNIQUE_VARNAME(__defer_i_)++, (__freeFn(__ptr))))

#define DEFER_MULTI(__freeFn, __count, ...) \
for (bool UNIQUE_VARNAME(__defer_i_) = 0; UNIQUE_VARNAME(__defer_i_) == 0; (UNIQUE_VARNAME(__defer_i_)++, DeferExecuteVA(__freeFn, __count, __VA_ARGS__)))

#define DEFER_BREAK continue

inline void DeferExecuteVA(void (*func)(void*), const int argc, ...) {
	va_list args;
	va_start(args, argc);
	for (int i = 0; i < argc; i++) {
		func(va_arg(args, void*));
	}
	va_end(args);
}

BOOM_HEADER_END