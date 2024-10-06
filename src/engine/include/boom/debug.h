#pragma once

#include "defs.h"

BOOM_HEADER_START

#include <signal.h>

typedef enum DebugLevel {
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_WARNING,
	DEBUG_LEVEL_ERROR
} DebugLevel;

typedef struct DebugInfo {
	const char* file;
	int line;
	const char* function;
} DebugInfo;

BOOM_API void DebugLog(DebugInfo info, DebugLevel level, const char* fmt, ...);
BOOM_API void DebugLogV(DebugInfo info, DebugLevel level, const char* fmt, va_list args);

#ifndef NDEBUG
	#ifdef _WIN32
		#define DEBUG_BREAK raise(SIGABRT)
	#else
		#define DEBUG_BREAK raise(SIGINT)
	#endif
#else
	#define DEBUG_BREAK
#endif

BOOM_API void DebugCloseLogFile();

#define LogInfo  ((DebugInfo){ __FILE__, __LINE__, __func__ }), DEBUG_LEVEL_INFO
#define LogWarn  ((DebugInfo){ __FILE__, __LINE__, __func__ }), DEBUG_LEVEL_WARNING
#define LogError ((DebugInfo){ __FILE__, __LINE__, __func__ }), DEBUG_LEVEL_ERROR

#ifndef NDEBUG
	#define ASSERT(__expression) if (!(__expression)) { DEBUG_BREAK; DebugLog(LogWarn, "Assertion failed for expression \"%s\"", #__expression); }
	#define ASSERT_FATAL(__expression) if (!(__expression)) { DEBUG_BREAK; DebugLog(LogError, "Assertion failed for expression \"%s\"", #__expression); }
#else
	#define ASSERT(__expression)
	#define ASSERT_FATAL(__expression)
#endif

BOOM_HEADER_END