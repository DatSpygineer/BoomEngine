#pragma once

#include "cvar.h"
#include "debug.h"

BOOM_API void ConsoleInitialize();
BOOM_API void ConsoleShutdown();
BOOM_API void ConsoleExec(const char* command);
BOOM_API void ConsoleExecFile(const char* cfgPath);

BOOM_API void ConsoleAppendMessage(const char* message, DebugLevel level);
BOOM_API void ConsoleClearMessages();

BOOM_API char** ConsoleGetLines();