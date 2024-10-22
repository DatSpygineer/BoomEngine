#include "puli_fsys.h"

#include <errno.h>

errno_t pPathGetFilename(char* buffer, size_t maxBufSize, const char* path) {
	errno = 0;
	ssize_t idx = pStrFindLast(path, P_PATH_SEPARATOR);
	if (idx < 0) {
		return pStrCpy(buffer, maxBufSize, path);
	} else {
		return pStrCpy(buffer, maxBufSize, path + idx + 1);
	}
}
errno_t pPathGetFilenameNoExt(char* buffer, size_t maxBufSize, const char* path) {
	errno_t err = pPathGetFilename(buffer, maxBufSize, path);
	if (err != 0) {
		return err;
	}
	char* p =strrchr(buffer, '.');
	*p = '\0';
	return errno;
}
errno_t pPathGetExtension(char* buffer, size_t maxBufSize, const char* path) {
	errno = 0;
	ssize_t idx = pStrFindLast(path, '.');
	if (idx < 0) {
		return pStrCpy(buffer, maxBufSize, path + idx);
	}
	return pStrCpy(buffer, maxBufSize, "");
}

errno_t pPathAppend(char* dest, const char* src) {
	if (!pStrEndsWith(dest, P_PATH_SEPARATOR) && !pStrStartsWith(src, P_PATH_SEPARATOR)) {
		if (pStrCat(dest, P_PATH_MAX, P_PATH_SEPARATOR_STR) != 0) {
			return errno;
		}
	}
	return pStrCat(dest, P_PATH_MAX, src);
}
errno_t pPathParent(char* dest, const char* src) {
	ssize_t i = pStrFindLast(src, P_PATH_SEPARATOR);
	if (i < 0) {
		return pStrCpy(dest, P_PATH_MAX, src);
	}
	return pStrnCpy(dest, P_PATH_MAX, src, i);
}
errno_t pPathAbsolute(char* dest, const char* src) {
#ifdef _WIN32

#else
	errno = 0;
	realpath(src, dest);
	return errno;
#endif
}
errno_t pPathAbsoluteTo(char* dest, const char* src, const char* base) {
	const errno_t e = pPathAbsolute(dest, src);
	if (e != 0) {
		return e;
	}
	return pPathAppend(dest, src);
}
bool pPathExists(const char* path) {
	struct stat st;
	return stat(path, &st) == 0;
}
bool pPathIsDir(const char* path) {
	struct stat st;
	return stat(path, &st) == 0 && st.st_mode & S_IFDIR;
}
bool pPathIsFile(const char* path) {
	struct stat st;
	return stat(path, &st) == 0 && st.st_mode & S_IFREG;
}

bool pFileReadLine(FILE* f, char* line, size_t maxBufSize) {
	return pFileReadTok(f, line, maxBufSize, '\n');
}
bool pFileReadTok(FILE* f, char* line, size_t maxBufSize, char delim) {
	int c = fgetc(f);
	size_t remain = maxBufSize - strlen(line);
	while (c != EOF && remain > 0) {
		if (c == delim) {
			return true;
		}
		*line++ = (char)c;
		remain--;
		c = fgetc(f);
	}
	return false;
}