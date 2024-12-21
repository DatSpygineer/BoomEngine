#pragma once

#include "puli_str.h"

#include <sys/stat.h>

#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
	#include <shlwapi.h>

	#define P_PATH_MAX MAX_PATH
	#define P_PATH_SEPARATOR '\\'
	#define P_PATH_SEPARATOR_STR "\\"
#else
	#include <limits.h>
	#include <dirent.h>
	#include <fnmatch.h>
	#include <unistd.h>

	#define P_PATH_MAX PATH_MAX
	#define P_PATH_SEPARATOR '/'
	#define P_PATH_SEPARATOR_STR "/"
#endif

#define P_PATH_BUFFER(__name) char __name[PATH_MAX] = { 0 }

typedef struct PuliDirectoryIterator {
	const char* basedir;
#ifdef _WIN32
#else
	DIR* dir;
#endif
} PuliDirIterator;
typedef PuliDirIterator pdir_iterator_t;

PuliError pPathGetFilename(char* buffer, size_t maxBufSize, const char* path);
PuliError pPathGetFilenameNoExt(char* buffer, size_t maxBufSize, const char* path);
PuliError pPathGetExtension(char* buffer, size_t maxBufSize, const char* path);
PuliError pPathAppend(char* dest, const char* src);
PuliError pPathParent(char* dest, const char* src);
PuliError pPathAbsolute(char* dest, const char* src);
PuliError pPathAbsoluteTo(char* dest, const char* src, const char* base);

const char* pPathCurrentDir(char* path);
PuliError pPathSetCurrentDir(const char* path);

bool pPathExists(const char* path);
bool pPathIsDir(const char* path);
bool pPathIsFile(const char* path);
bool pPathMatchPattern(const char* path, const char* pattern);

PuliError pPathCopy(const char* dest, const char* src);
PuliError pPathMove(const char* dest, const char* src);
PuliError pPathRemove(const char* path);

bool pFileReadLine(FILE* f, char* line, size_t maxBufSize);
bool pFileReadTok(FILE* f, char* line, size_t maxBufSize, char delim);

PuliError pDirectoryCreate(const char* path, bool recursive);
PuliError pDirectoryDelete(const char* path, bool recursive);
PuliError pDirectoryCopy(const char* dest, const char* src, bool recursive);
PuliError pDirectoryMove(const char* dest, const char* src, bool recursive);

PuliDirIterator* pDirIteratorOpen(const char* path);
bool pDirIteratorGetEntry(PuliDirIterator* it, char* buffer, bool fullpath);
void pDirIteratorClose(PuliDirIterator** it);