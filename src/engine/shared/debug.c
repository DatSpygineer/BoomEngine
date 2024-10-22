#include "debug.h"

#include <stdarg.h>
#include <time.h>

static FILE* g_debug_file = NULL;

static FILE* setup_debugfile() {
	if (g_debug_file != NULL) {
		return g_debug_file;
	}

	const time_t t = time(NULL);
	const struct tm tm = *localtime(&t);
	char filename[P_PATH_MAX];
	snprintf(filename, sizeof(filename), "log_%04d_%02d_%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	g_debug_file = fopen(filename, "a+");
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
		level == 0 ? "INFO" : (level == 1 ? "WARN" : "ERR "),
		info.filename, info.line, info.funcname
	);

	va_list args;
	va_start(args, fmt);
	vsnprintf(message + offset, 4096 - offset, fmt, args);
	va_end(args);

	puts(message);
	if (f != NULL) {
		fputs(message, f);
		fputc('\n', f);
	}

	if (level == 2) {
		pMessageBox("Fatal error", message, P_MSGBOX_ERROR, P_MSGBOX_BUTTON_OK);
	}
}

void debug_close_log() {
	if (g_debug_file != NULL) {
		fclose(g_debug_file);
	}
}