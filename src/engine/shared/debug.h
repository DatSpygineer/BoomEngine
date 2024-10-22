#pragma once

#include "defs.h"

BOOM_HEADER_BEGIN

typedef struct {
	const char* filename;
	int line;
	const char* funcname;
} DebugLogInfo;

#define LogInfo  (DebugLogInfo){ __FILE__, __LINE__, __func__ }, 0
#define LogWarn  (DebugLogInfo){ __FILE__, __LINE__, __func__ }, 1
#define LogError (DebugLogInfo){ __FILE__, __LINE__, __func__ }, 2

BOOM_API void debug_log(DebugLogInfo info, int level, const char* fmt, ...);
BOOM_API void debug_close_log();

#define DevMsg(...)  debug_log(LogInfo, __VA_ARGS__)
#define DevWarn(...) debug_log(LogWarn, __VA_ARGS__)
#define DevErr(...)  debug_log(LogError, __VA_ARGS__)

#define Assert(__x)      if (!(__x)) { DevWarn("Assertion failed: %s", #__x); }
#define AssertFatal(__x) if (!(__x)) { DevErr ("Assertion failed: %s", #__x); }

BOOM_HEADER_END