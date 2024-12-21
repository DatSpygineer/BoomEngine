#include "debug.h"

#include <paths.h>
#include <stdarg.h>
#include <time.h>

static FILE* g_debug_file = NULL;

static FILE* setup_debugfile() {
	if (g_debug_file != NULL) {
		return g_debug_file;
	}

	const time_t t = time(NULL);
	const struct tm tm = *localtime(&t);
	char full_path[P_PATH_MAX] = { 0 };
	pStrCpy(full_path, P_PATH_MAX, get_root_path());
	pPathAppend(full_path, "log");

	if (!pPathExists(full_path)) {
		pDirectoryCreate(full_path, true);
	}

	char filename[128] = { 0 };
	snprintf(filename, 128, "log_%04d_%02d_%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	pPathAppend(full_path, filename);

	g_debug_file = fopen(full_path, "a+");
	return g_debug_file;
}

void debug_log(DebugLogInfo info, int level, const char* fmt, ...) {
	FILE* f = setup_debugfile();
	const time_t t = time(NULL);
	const struct tm tm = *localtime(&t);

	char message[4096] = { 0 };
	int offset = snprintf(message, 4096,
		"%04d.%02d.%02d %02d:%02d:%02d - [%s] \"%s\":%d (%s) => ",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
		level == 0 ? "INFO" : (level == 1 ? "WARN" : "ERROR"),
		info.filename, info.line, info.funcname
	);

	va_list args;
	va_start(args, fmt);
	vsnprintf(message + offset, 4096 - offset, fmt, args);
	va_end(args);

	va_start(args, fmt);
	if (level == 2) {
		pMessageBoxFormatV("Fatal error!", MSGBOX_ERROR, MSGBOX_OK, fmt, args);
	}
	va_end(args);

	puts(message);
	if (f != NULL) {
		fputs(message, f);
		fputc('\n', f);
	}

}

void debug_close_log() {
	if (g_debug_file != NULL) {
		fclose(g_debug_file);
	}
}