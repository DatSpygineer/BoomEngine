#pragma once

#include "puli_str.h"

#include <sys/stat.h>

#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>

	#define P_PATH_MAX MAX_PATH
	#define P_PATH_SEPARATOR '\\'
	#define P_PATH_SEPARATOR_STR "\\"
#else
	#include <limits.h>
	#include <dirent.h>
	#include <glob.h>

	#define P_PATH_MAX PATH_MAX
	#define P_PATH_SEPARATOR '/'
	#define P_PATH_SEPARATOR_STR "/"
#endif

errno_t pPathGetFilename(char* buffer, size_t maxBufSize, const char* path);
errno_t pPathGetFilenameNoExt(char* buffer, size_t maxBufSize, const char* path);
errno_t pPathGetExtension(char* buffer, size_t maxBufSize, const char* path);
errno_t pPathAppend(char* dest, const char* src);
errno_t pPathParent(char* dest, const char* src);
errno_t pPathAbsolute(char* dest, const char* src);
errno_t pPathAbsoluteTo(char* dest, const char* src, const char* base);

bool pPathExists(const char* path);
bool pPathIsDir(const char* path);
bool pPathIsFile(const char* path);

bool pFileReadLine(FILE* f, char* line, size_t maxBufSize);
bool pFileReadTok(FILE* f, char* line, size_t maxBufSize, char delim);