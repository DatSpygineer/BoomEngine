#pragma once

#if defined(CLIENT_DLL) && defined(SERVER_DLL)
	#error Macro "CLIENT_DLL" and "SERVER_DLL" shall not be defined the same time!
#endif

#ifdef __cplusplus
	#define BOOM_HEADER_BEGIN extern "C" {
	#define BOOM_HEADER_END }

	#include <cstdio>
	#include <cstdlib>
	#include <cstdint>
	#include <cstring>
#else
	#define BOOM_HEADER_BEGIN
	#define BOOM_HEADER_END

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <stdbool.h>
	#include <string.h>
#endif

#include "puli.h"

BOOM_HEADER_BEGIN

#include <errno.h>

#ifdef BOOM_STATIC
	#define BOOM_API
#else
	#ifdef BOOM_EXPORTS
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
#endif

BOOM_HEADER_END