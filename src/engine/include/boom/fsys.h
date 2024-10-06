#pragma once

#include "str.h"

#ifdef _WIN32
	#define _VA_LIST_DEFINED
	#define StrEqualsNoCase(__lhs, __rhs) (_stricmp(__lhs, __rhs) == 0)
	#define StrEqualsNoCaseN(__lhs, __rhs, __n) (_strnicmp(__lhs, __rhs, __n) == 0)
	#define BOOM_PATH_MAX MAX_PATH
	#define BOOM_PATH_SEPARATOR '\\'
	#include <Windows.h>
	#include <direct.h>
#else
#define StrEqualsNoCase(__lhs, __rhs) (strcasecmp(__lhs, __rhs) == 0)
	#define StrEqualsNoCaseN(__lhs, __rhs, __n) (strncasecmp(__lhs, __rhs, __n) == 0)
	#define BOOM_PATH_MAX PATH_MAX
	#define BOOM_PATH_SEPARATOR '/'
	#include <dirent.h>
#endif

#define PathDecl(__name) char __name[BOOM_PATH_MAX]

BOOM_API char* PathAppend(char* trg, const char* src);
BOOM_API char* PathAppendF(char* trg, const char* fmt, ...);
BOOM_API char* PathAppendVF(char* trg, const char* fmt, va_list args);
BOOM_API char* PathJoin(char* trg, int argc, ...);
BOOM_API char* PathJoinV(char* trg, int argc, va_list args);

BOOM_API char* PathAbsolute(char* trg, const char* src);
BOOM_API char* PathAbsoluteTo(char* trg, const char* src, const char* basedir);
BOOM_API char* PathGetFileName(char* trg, size_t maxBufSize, const char* src);
BOOM_API char* PathGetFileNameNoExt(char* trg, size_t maxBufSize, const char* src);
BOOM_API char* PathGetExtension(char* trg, size_t maxBufSize, const char* src);
BOOM_API char* PathGetParent(char* trg, const char* src);
BOOM_API char* PathParent(char* trg);

BOOM_API bool PathExists(const char* path);
BOOM_API bool PathIsFile(const char* path);
BOOM_API bool PathIsDir(const char* path);
BOOM_API bool PathCreateDir(const char* path, bool recursive);
BOOM_API bool PathRemove(const char* path);
BOOM_API bool PathRemoveDir(const char* path, bool recursive);
BOOM_API bool PathCopy(const char* to, const char* from);
BOOM_API bool PathMove(const char* to, const char* from);
BOOM_API bool PathIsRelative(const char* path);

typedef struct DirectoryIterator {
	const char* pBasePath;
#ifdef _WIN32
	HANDLE hFile;
	WIN32_FIND_DATA ffd;
#else
	DIR* pDir;
#endif
} DirectoryIterator;


BOOM_API DirectoryIterator* DirectoryIteratorCreate(const char* path);
#ifdef _WIN32
BOOM_API bool DirectoryIteratorNext(char* trg, DirectoryIterator* it);
BOOM_API bool DirectoryIteratorNextFileName(char* trg, size_t maxBufSize, DirectoryIterator* it);
#else
BOOM_API bool DirectoryIteratorNext(char* trg, const DirectoryIterator* it);
BOOM_API bool DirectoryIteratorNextFileName(char* trg, size_t maxBufSize, const DirectoryIterator* it);
#endif
BOOM_API void DirectoryIteratorFree(DirectoryIterator** it);

BOOM_API size_t FileReadToken(FILE* f, char* line, char delim, size_t maxBufSize);
#define FileReadLine(__f, __line, __maxBufSize) FileReadToken(__f, __line, '\n', __maxBufSize)

BOOM_API FILE* FileOpenRead(const char* path);
BOOM_API FILE* FileOpenWrite(const char* path);
BOOM_API FILE* FileOpenAppend(const char* path, bool allowCreate);

BOOM_API FILE* FileOpenCfgRead(const char* path);
BOOM_API FILE* FileOpenCfgWrite(const char* path);
BOOM_API FILE* FileOpenCfgAppend(const char* path, bool allowCreate);

BOOM_API size_t FileGetSize(FILE* f);

BOOM_API void* FileAllocAndReadAll(FILE* f);

#define FileSeek(__f, __pos, __origin) fseek(__f, __pos, __origin)
#define FileTell(__f) ftell(__f)
#define FileEOF(__f) feof(__f)
#define FileError(__f) ferror(__f)
#define FileWrite(__trg, __size, __count, __f) fwrite(__trg, __size, __count, __f)
#define FileRead(__trg, __size, __count, __f) fread(__trg, __size, __count, __f)
#define FileFlush(__f) fflush(__f)
#define FileClose(__f) if ((__f) != NULL) { fclose(__f) }