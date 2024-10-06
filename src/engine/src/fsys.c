#include "boom/fsys.h"
#include "boom/entry.h"

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

#ifdef _WIN32
	#ifndef S_ISDIR
		#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
	#endif
	#ifndef S_ISREG
		#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
	#endif
#else
	#include <fcntl.h>
#endif

char* PathAppend(char* trg, const char* src) {
	if (trg == NULL) return NULL;
	if (!StrEndWithChar(trg, BOOM_PATH_SEPARATOR) && !StrStartWithChar(src, BOOM_PATH_SEPARATOR)) {
		if (StrAppendChar(trg, BOOM_PATH_MAX, BOOM_PATH_SEPARATOR) == NULL) {
			return NULL;
		}
	}
	return StrAppend(trg, BOOM_PATH_MAX, src);
}

char* PathAppendF(char* trg, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char* p = PathAppendVF(trg, fmt, args);
	va_end(args);
	return p;
}
char* PathAppendVF(char* trg, const char* fmt, va_list args) {
	PathDecl(temp) = { 0 };
	vsnprintf(temp, BOOM_PATH_MAX, fmt, args);
	return PathAppend(trg, temp);
}

char* PathJoin(char* trg, int argc, ...) {
	va_list args;
	va_start(args, argc);
	char* p = PathJoinV(trg, argc, args);
	va_end(args);
	return p;
}
char* PathJoinV(char* trg, int argc, va_list args) { // NOLINT: args must be non-const
	if (argc == 0) return NULL;

	for (size_t i = 0; i < argc; i++) {
		const char* arg = va_arg(args, const char*);
		if (PathAppend(trg, arg) == NULL) {
			return NULL;
		}
	}
	return trg;
}

char* PathAbsolute(char* trg, const char* src) {
	return PathAbsoluteTo(trg, src, BoomBaseDirectory());
}
char* PathAbsoluteTo(char* trg, const char* src, const char* basedir) {
	if (PathIsRelative(src)) {
		StrCpy(trg, BOOM_PATH_MAX, basedir);
		return PathAppend(trg, src);
	}
	return StrCpy(trg, BOOM_PATH_MAX, src);
}
char* PathGetFileName(char* trg, size_t maxBufSize, const char* src) {
	const ssize_t idx = StrFindLastChar(src, BOOM_PATH_SEPARATOR);
	if (idx > -1) {
		return StrCpy(trg, maxBufSize, src + idx + 1);
	}
	return StrCpy(trg, maxBufSize, src);
}
char* PathGetFileNameNoExt(char* trg, size_t maxBufSize, const char* src) {
	const ssize_t start = StrFindLastChar(src, BOOM_PATH_SEPARATOR);
	if (start == -1) {
		return StrCpy(trg, maxBufSize, src);
	}
	const ssize_t end = StrFindLastChar(src + start, '.');
	return StrCpyN(trg, maxBufSize, src + start, end);
}
char* PathGetExtension(char* trg, size_t maxBufSize, const char* src) {
	const ssize_t idx = StrFindLastChar(src, '.');
	if (idx > -1) {
		return StrCpy(trg, maxBufSize, src + idx + 1);
	}
	return StrCpy(trg + idx, maxBufSize, src);
}
char* PathGetParent(char* trg, const char* src) {
	const ssize_t idx = StrFindLastChar(src, BOOM_PATH_SEPARATOR);
	if (idx > -1) {
		return StrCpyN(trg, BOOM_PATH_MAX, src, idx);
	}
	return NULL;
}
char* PathParent(char* trg) {
	if (trg == NULL) return NULL;

	const ssize_t idx = StrFindLastChar(trg, BOOM_PATH_SEPARATOR);
	if (idx > -1) {
		char* p = trg + idx;
		while (*p != '\0') {
			*p = '\0';
			p++;
		}
	}
	return trg;
}

bool PathExists(const char* path) {
	struct stat st;
	return stat(path, &st) == 0;
}
bool PathIsFile(const char* path) {
	struct stat st;
	return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}
bool PathIsDir(const char* path) {
	struct stat st;
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}
bool PathCreateDir(const char* path, const bool recursive) { // NOLINT: Recursive call is intended and checked
	if (PathIsDir(path)) return false;

	if (recursive) {
		PathDecl(temp) = { 0 };
		if (PathGetParent(temp, path) == NULL && !PathIsDir(path)) {
			return false;
		}
		if (!PathIsDir(temp)) {
			PathCreateDir(temp, true);
		}
	}
#ifdef _WIN32
	return _mkdir(path) == 0;
#else
	return mkdir(path, 0777) == 0;
#endif
}
bool PathRemove(const char* path) {
	return remove(path) == 0;
}
bool PathRemoveDir(const char* path, const bool recursive) { // NOLINT: Recursive call is intended and checked
	if (recursive) {
		PathDecl(temp) = { 0 };
		DirectoryIterator* it = DirectoryIteratorCreate(path);
		while (DirectoryIteratorNext(temp, it)) {
			if (PathIsDir(temp)) {
				PathRemoveDir(temp, recursive);
			} else {
				PathRemove(temp);
			}
		}
		DirectoryIteratorFree(&it);
	}
#ifdef _WIN32
	return RemoveDirectoryA(path) != 0;
#else
	return rmdir(path) == 0;
#endif
}

bool PathCopy(const char* to, const char* from) { // NOLINT: Recursive call is intended and checked
	if (PathIsDir(from)) {
		bool result = true;

		if (PathIsDir(to)) {
			if (!PathCreateDir(to, true)) {
				return false;
			}
		}

		PathDecl(tempPathTo) = { 0 };
		PathDecl(tempPathFrom) = { 0 };
		char tempName[256] = { 0 };

		DirectoryIterator* it = DirectoryIteratorCreate(from);
		while (DirectoryIteratorNextFileName(tempName, 256, it)) {
			StrCpy(tempPathTo, BOOM_PATH_MAX, to);
			StrCpy(tempPathFrom, BOOM_PATH_MAX, from);
			PathAppend(tempPathTo, tempName);
			PathAppend(tempPathFrom, tempName);
			if (!PathCopy(tempPathTo, tempPathFrom)) {
				result = false;
				break;
			}
		}
		DirectoryIteratorFree(&it);
		return result;
	} else {
#ifdef _WIN32
		return CopyFile(from, to, FALSE) != 0;
#else
		const int f_from = open(from, O_RDONLY, 0777);
		if (f_from == -1) {
			return false;
		}
		const int f_to = open(to, O_WRONLY | O_CREAT, 0777);
		if (f_to == -1) {
			close(f_from);
			return false;
		}

		uint8_t byte;
		while (read(f_from, &byte, 1) <= 0) {
			write(f_to, &byte, 1);
		}
		close(f_to);
		close(f_from);
		return true;
#endif
	}
}
bool PathMove(const char* to, const char* from) {
	if (PathIsDir(from)) {
		if (PathCopy(to, from)) {
			return PathRemoveDir(from, true);
		}
		return false;
	}
#ifdef _WIN32
	return MoveFile(from, to) != 0;
#else
	return rename(from, to) == 0;
#endif
}

DirectoryIterator* DirectoryIteratorCreate(const char* path) {
#ifdef _WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hnd = FindFirstFileA(path, &ffd);
	if (hnd == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DirectoryIterator* it = (DirectoryIterator*)malloc(sizeof(DirectoryIterator));
	it->ffd = ffd;
	it->hFile = hnd;
	it->pBasePath = path;
	return it;
#else
	DIR* dir = opendir(path);
	if (dir == NULL) {
		return NULL;
	}
	DirectoryIterator* it = (DirectoryIterator*)malloc(sizeof(DirectoryIterator));
	it->pBasePath = path;
	it->pDir = dir;
	return it;
#endif
}


#ifdef _WIN32
bool DirectoryIteratorNext(char* trg, DirectoryIterator* it) {
	if (StrCpy(trg, BOOM_PATH_MAX, it->pBasePath) != NULL && PathAppend(trg, it->ffd.cFileName) != NULL) {
		return FindNextFileA(it->hFile, &it->ffd) != 0;
	}
	return false;
#else
	bool DirectoryIteratorNext(char* trg, const DirectoryIterator* it) {
	if (it->pDir == NULL) {
		return false;
	}
	struct dirent* entry = readdir(it->pDir);
	if (entry == NULL) {
		return false;
	}
	return StrCpy(trg, BOOM_PATH_MAX, it->pBasePath) != NULL && PathAppend(trg, entry->d_name) != NULL;
#endif
}

#ifdef _WIN32
bool DirectoryIteratorNextFileName(char* trg, const size_t maxBufSize, DirectoryIterator* it) {
	if (StrCpy(trg, maxBufSize, it->ffd.cFileName) != NULL) {
		return FindNextFileA(it->hFile, &it->ffd) != 0;
	}
	return false;
#else
	bool DirectoryIteratorNextFileName(char* trg, const size_t maxBufSize, const DirectoryIterator* it) {
	if (it->pDir == NULL) {
		return false;
	}
	struct dirent* entry = readdir(it->pDir);
	if (entry == NULL) {
		return false;
	}
	return StrCpy(trg, maxBufSize, entry->d_name) != NULL;
#endif
}
void DirectoryIteratorFree(DirectoryIterator** it) {
#ifdef _WIN32
	if ((*it)->hFile != INVALID_HANDLE_VALUE) {
		FindClose((*it)->hFile);
	}
#else
	if (it != NULL && *it != NULL) {
		if ((*it)->pDir) closedir((*it)->pDir);
		*it = NULL;
	}
#endif
}

size_t FileReadToken(FILE* f, char* line, char delim, size_t maxBufSize) {
	int c = fgetc(f);
	size_t read_c = 0;
	while (c != delim && c != EOF && read_c < maxBufSize) {
		line[read_c++] = (char)c;
		c = fgetc(f);
	}
	line[read_c] = '\0';
	return read_c;
}

FILE* FileOpenRead(const char* path) {
	PathDecl(fullpath);
	PathAbsolute(fullpath, path);
	return fopen(path, "r");
}
FILE* FileOpenWrite(const char* path) {
	PathDecl(fullpath);
	PathAbsolute(fullpath, path);
	return fopen(path, "w");
}
FILE* FileOpenAppend(const char* path, bool allowCreate) {
	PathDecl(fullpath);
	PathAbsolute(fullpath, path);
	return fopen(path, allowCreate ? "a+" : "a");
}

FILE* FileOpenCfgRead(const char* path) {
	PathDecl(fullpath);
	PathAbsoluteTo(fullpath, path, BoomCfgDirectory());
	return fopen(path, "r");
}
FILE* FileOpenCfgWrite(const char* path) {
	PathDecl(fullpath);
	PathAbsoluteTo(fullpath, path, BoomCfgDirectory());
	return fopen(path, "w");
}
FILE* FileOpenCfgAppend(const char* path, bool allowCreate) {
	PathDecl(fullpath);
	PathAbsoluteTo(fullpath, path, BoomCfgDirectory());
	return fopen(path, allowCreate ? "a+" : "a");
}

size_t FileGetSize(FILE* f) {
	long oPos = FileTell(f);
	FileSeek(f, 0, SEEK_END);
	long size = FileTell(f);
	FileSeek(f, oPos, SEEK_SET);
	return (size_t)size;
}

void* FileAllocAndReadAll(FILE* f) {
	size_t size = FileGetSize(f);
	void* buffer = malloc(size);
	if (buffer != NULL) FileRead(buffer, 1, size, f);
	return buffer;
}

bool PathIsRelative(const char* path) {
#ifdef _WIN32
	return !(StrLen(path) > 1 && CharIsLetter(path[0]) && path[1] == ':');
#else
	return !StrStartWithChar(path, '/');
#endif
}
