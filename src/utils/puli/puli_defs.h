#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef __GNUC__
	#if SIZE_MAX == UINT32_MAX
		typedef int32_t ssize_t;
		#define SSIZE_MAX INT32_MAX
	#else
		typedef int64_t ssize_t;
		#define SSIZE_MAX INT64_MAX
	#endif
#endif

#ifndef errno_t
	#define errno_t int
#endif