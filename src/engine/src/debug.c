#include "boom/debug.h"
#include "boom/entry.h"
#include "boom/fsys.h"
#include "boom/console.h"

#include <stdio.h>
#include <time.h>
#include <boom/str.h>

FILE* g_pFile = NULL;

static FILE* OpenLogFile() {
	if (g_pFile == NULL) {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		PathDecl(temp) = { 0 };
		StrCpy(temp, BOOM_PATH_MAX, BoomBaseDirectory());
		PathAppend(temp, "logs");
		PathAppendF(temp, "log_%04d.%02d.%02d.txt", tm.tm_year, tm.tm_mon, tm.tm_mday);

		if (PathExists(temp)) {
			g_pFile = fopen(temp, "a+");
		} else {
			g_pFile = fopen(temp, "w");
		}
	}
	return g_pFile;
}

void DebugLog(DebugInfo info, DebugLevel level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	DebugLogV(info, level, fmt, args);
	va_end(args);
}
void DebugLogV(DebugInfo info, DebugLevel level, const char* fmt, va_list args) {
	FILE* f = OpenLogFile();
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);

#ifdef _WIN32
	HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;
	GetConsoleScreenBufferInfo(hnd, &csbInfo);
	WORD wDefaultAttrib = csbInfo.wAttributes;

	switch (level) {
		case DEBUG_LEVEL_WARNING: SetConsoleTextAttribute(hnd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		case DEBUG_LEVEL_ERROR: SetConsoleTextAttribute(hnd, FOREGROUND_RED | FOREGROUND_INTENSITY); break;
		default: /* Keep default colour */ break;
	}
#else
	switch (level) {
		case DEBUG_LEVEL_WARNING: printf("\x1B[33;1m"); break;
		case DEBUG_LEVEL_ERROR: printf("\x1B[31;1m"); break;
		default: /* Keep default colour */ break;
	}
#endif

	printf("%04d.%02d.%02d %02d:%02d:%02d [%s] - \"%s\":%d (%s) => ",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
		level == DEBUG_LEVEL_ERROR ? "ERROR" : (level == DEBUG_LEVEL_WARNING ? "WARNING" : "INFO"),
		info.file, info.line, info.function
	);

	char temp[1024] = { 0 };
	vsnprintf(temp, 1024, fmt, args);

	printf("%s", temp);
#if defined(_WIN32) && !defined(NDEBUG)
	char message[4096] = { 0 };
	sprintf_s(message, 4096, "%04d.%02d.%02d %02d:%02d:%02d [%s] - \"%s\":%d (%s) => %s",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
		level == DEBUG_LEVEL_ERROR ? "ERROR" : (level == DEBUG_LEVEL_WARNING ? "WARNING" : "INFO"),
		info.file, info.line, info.function,
		temp
	);
	OutputDebugStringA(message);
#endif

#ifdef _WIN32
	SetConsoleTextAttribute(hnd, wDefaultAttrib);
#else
	if (level != DEBUG_LEVEL_INFO) {
		printf("\x1B[0m\n");
	} else {
#endif
		printf("\n");
#ifndef _WIN32
	}
#endif
	if (f != NULL) {
		fprintf(f, "%04d.%02d.%02d %02d:%02d:%02d [%s] - \"%s\":%d (%s) => ",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
			level == DEBUG_LEVEL_ERROR ? "ERROR" : (level == DEBUG_LEVEL_WARNING ? "WARNING" : "INFO"),
			info.file, info.line, info.function
		);
		fprintf(f, "%s\n", temp);
#ifndef NDEBUG
		fflush(f);
#endif
	}

	ConsoleAppendMessage(temp, level);

	if (level == DEBUG_LEVEL_ERROR) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", temp, NULL);
		BoomShutdown();
		exit(-1);
	}
}

void DebugCloseLogFile() {
	if (g_pFile != NULL) {
		fclose(g_pFile);
		g_pFile = NULL;
	}
}