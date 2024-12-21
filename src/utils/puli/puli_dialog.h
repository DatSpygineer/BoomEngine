#pragma once

#include "puli_defs.h"

#ifndef __cplusplus
	#include <stdbool.h>
#endif

typedef enum MessageBoxType {
	MSGBOX_INFO,
	MSGBOX_WARNING,
	MSGBOX_ERROR,
	MSGBOX_QUESTION,
} MessageBoxType;

typedef enum MessageBoxButtons {
	MSGBOX_OK,
	MSGBOX_OK_CANCEL,
	MSGBOX_YES_NO,
	MSGBOX_YES_NO_CANCEL
} MessageBoxButtons;

typedef enum MessageBoxResult {
	RESULT_OK,
	RESULT_CANCEL,
	RESULT_YES,
	RESULT_NO,
	RESULT_UNKNOWN
} MessageBoxResult;

typedef struct FileSelectFilter {
#ifdef _WIN32
	const wchar_t* name;
	const wchar_t* pattern;
#else
	const char* name;
	const char* pattern;
#endif
} FileSelectFilter;

#if !defined(_WIN32) && !defined(TEXT)
	#define TEXT(x) x
#endif

extern void pSetupDialogs(int argc, char** argv);
extern void pDisposeDialogs();

extern MessageBoxResult pMessageBox(const char* title, const char* message, MessageBoxType type, MessageBoxButtons buttons);
extern MessageBoxResult pMessageBoxFormat(const char* title, MessageBoxType type, MessageBoxButtons buttons, const char* fmt, ...);
extern MessageBoxResult pMessageBoxFormatV(const char* title, MessageBoxType type, MessageBoxButtons buttons, const char* fmt, va_list args);

extern size_t pOpenFileDialog(char** result, const char* title, int max_count, const FileSelectFilter* filters, size_t filter_count);
extern size_t pOpenFileDialogMultiple(char** result, const char* title, int max_count, const FileSelectFilter* filters, size_t filter_count);
extern size_t pSaveFileDialog(char* result, const char* title, const FileSelectFilter* filters, size_t filter_count);
extern size_t pOpenDirDialog(char* result, const char* title);