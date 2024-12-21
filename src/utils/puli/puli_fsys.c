#include "puli_fsys.h"

#include <errno.h>

PuliError pPathGetFilename(char* buffer, size_t maxBufSize, const char* path) {
	errno = 0;
	ssize_t idx = pStrFindLast(path, P_PATH_SEPARATOR);
	if (idx < 0) {
		return pStrCpy(buffer, maxBufSize, path);
	} else {
		return pStrCpy(buffer, maxBufSize, path + idx + 1);
	}
}
PuliError pPathGetFilenameNoExt(char* buffer, size_t maxBufSize, const char* path) {
	PuliError err = pPathGetFilename(buffer, maxBufSize, path);
	if (err != P_OK) {
		return err;
	}

	char* p = strrchr(buffer, '.');
	if (p != NULL) {
		*p = '\0';
	}

	return P_OK;
}
PuliError pPathGetExtension(char* buffer, size_t maxBufSize, const char* path) {
	errno = 0;
	ssize_t idx = pStrFindLast(path, '.');
	if (idx < 0) {
		return pStrCpy(buffer, maxBufSize, path + idx);
	}
	return pStrCpy(buffer, maxBufSize, "");
}

PuliError pPathAppend(char* dest, const char* src) {
	if (!pStrEndsWith(dest, P_PATH_SEPARATOR) && !pStrStartsWith(src, P_PATH_SEPARATOR)) {
		if (pStrCat(dest, P_PATH_MAX, P_PATH_SEPARATOR_STR) != 0) {
			return errno;
		}
	}
	return pStrCat(dest, P_PATH_MAX, src);
}
PuliError pPathParent(char* dest, const char* src) {
	const ssize_t i = pStrFindLast(src, P_PATH_SEPARATOR);
	if (i < 0) {
		return pStrCpy(dest, P_PATH_MAX, src);
	}
	return pStrnCpy(dest, P_PATH_MAX, src, i);
}
PuliError pPathAbsolute(char* dest, const char* src) {
#ifdef _WIN32
	GetFullPathNameA(src, P_PATH_MAX, dest, NULL);
#else
	errno = 0;
	realpath(src, dest);
	return errno;
#endif
}
PuliError pPathAbsoluteTo(char* dest, const char* src, const char* base) {
	const PuliError e = pPathAbsolute(dest, src);
	if (e != 0) {
		return e;
	}
	return pPathAppend(dest, src);
}

const char* pPathCurrentDir(char* path) {
#ifdef _WIN32
	return _getcwd(path, P_PATH_MAX);
#else
	return getcwd(path, P_PATH_MAX);
#endif
}
PuliError pPathSetCurrentDir(const char* path) {
#ifdef _WIN32
	return _chdir(path);
#else
	return chdir(path);
#endif
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

bool pPathMatchPattern(const char* path, const char* pattern) {
#ifdef _WIN32
	return PathMatchSpecA(path, pattern) != 0;
#else
	return fnmatch(path, pattern, 0) == 0;
#endif
}

PuliError pPathCopy(const char* dest, const char* src) {
	if (pPathIsFile(src)) {
		FILE* fdest = fopen(dest, "w");
		FILE* fsrc = fopen(src, "r");
		if (fdest == NULL || fsrc == NULL) {
			return P_ERROR_NOT_FOUND;
		}

		uint8_t temp;
		while (!feof(fsrc)) {
			if (fread(&temp, 1, 1, fsrc) < 1) {
				break;
			}
			fwrite(&temp, 1, 1, fdest);
		}
		return P_OK;
	}
	return P_ERROR_NOT_A_FILE;
}
PuliError pPathMove(const char* dest, const char* src) {
	return rename(dest, src);
}
PuliError pPathRemove(const char* path) {
	return remove(path);
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

PuliError pDirectoryCreate(const char* path, bool recursive) {
	errno = 0;
	if (recursive) {
		P_PATH_BUFFER(base);
		if (pPathParent(base, path) == P_OK && pStrLen(base) > 0) {
			if (!pPathExists(base)) {
				PuliError err;
				if ((err = pDirectoryCreate(base, recursive)) != P_OK) {
					return err;
				}
			}
		}
	}
#ifdef _WIN32
	int err = _mkdir(path);
#else
	int err = mkdir(path, 0777);
#endif

	if (err == 0) {
		return P_OK;
	}

	return err;
}
PuliError pDirectoryDelete(const char* path, bool recursive) {
	if (recursive) {
		errno = 0;
		PuliDirIterator* it = pDirIteratorOpen(path);
		if (it == NULL) {
			return errno == 0 ? P_ERROR_IO_FAILURE : errno;
		}

		P_PATH_BUFFER(buffer);
		PuliError err;
		while (pDirIteratorGetEntry(it, buffer, true)) {
			if (pPathIsDir(buffer)) {
				err = pDirectoryDelete(buffer, recursive);
			} else {
				err = pPathRemove(buffer);
			}
			if (err != 0) {
				pDirIteratorClose(&it);
				return err;
			}
		}
		pDirIteratorClose(&it);
	}
	return pPathRemove(path);
}

PuliError pDirectoryCopy(const char* dest, const char* src, bool recursive) {
	errno = 0;

	PuliError err = P_OK;

	if (!pPathExists(dest)) {
		if ((err = pDirectoryCreate(dest, recursive)) != P_OK) {
			return err;
		}
	}

	PuliDirIterator* it = pDirIteratorOpen(src);
	char filename[256] = { 0 };
	P_PATH_BUFFER(src_path);
	P_PATH_BUFFER(dest_path);
	while (pDirIteratorGetEntry(it, filename, false)) {
		pStrCpy(src_path, PATH_MAX, src);
		pPathAppend(src_path, filename);
		pStrCpy(dest_path, PATH_MAX, dest);
		pPathAppend(dest_path, filename);

		if (pPathIsDir(src_path)) {
			if (recursive) {
				if ((err = pDirectoryCopy(src_path, dest_path, recursive)) != P_OK) {
					goto DIR_CPY_END;
				}
			}
		} else {
			if ((err = pPathCopy(dest_path, src_path)) != P_OK) {
				goto DIR_CPY_END;
			}
		}
	}

DIR_CPY_END:
	pDirIteratorClose(&it);
	return err;
}
PuliError pDirectoryMove(const char* dest, const char* src, bool recursive) {
	errno = 0;

	PuliError err = P_OK;

	if (!pPathExists(dest)) {
		if ((err = pDirectoryCreate(dest, recursive)) != P_OK) {
			return err;
		}
	}

	PuliDirIterator* it = pDirIteratorOpen(src);
	char filename[256] = { 0 };
	P_PATH_BUFFER(src_path);
	P_PATH_BUFFER(dest_path);
	while (pDirIteratorGetEntry(it, filename, false)) {
		pStrCpy(src_path, PATH_MAX, src);
		pPathAppend(src_path, filename);
		pStrCpy(dest_path, PATH_MAX, dest);
		pPathAppend(dest_path, filename);

		if (pPathIsDir(src_path)) {
			if (recursive) {
				if ((err = pDirectoryMove(src_path, dest_path, recursive)) != P_OK) {
					goto DIR_CPY_END;
				}
			} else {
				err = P_ERROR_NOT_A_FILE;
				goto DIR_CPY_END;
			}
		} else {
			if ((err = pPathMove(dest_path, src_path)) != P_OK) {
				goto DIR_CPY_END;
			}
		}
	}

DIR_CPY_END:
	pDirIteratorClose(&it);
	return err;
}

PuliDirIterator* pDirIteratorOpen(const char* path) {
	PuliDirIterator* it = (PuliDirIterator*)malloc(sizeof(PuliDirIterator));
	if (it != NULL) {
		it->basedir = path;
#ifdef _WIN32

#else
		it->dir = opendir(path);
		if (it->dir == NULL) {
			free(it);
			return NULL;
		}
#endif
	}
	return it;
}

bool pDirIteratorGetEntry(PuliDirIterator* it, char* buffer, bool fullpath) {
	if (it == NULL) return false;

#ifdef _WIN32
#else
	struct dirent* entry;
	if ((entry = readdir(it->dir)) == NULL) {
		return false;
	}
	if (fullpath) {
		pStrCpy(buffer, P_PATH_MAX, it->basedir);
		pPathAppend(buffer, entry->d_name);
	} else {
		pStrCpy(buffer, P_PATH_MAX, entry->d_name);
	}
	return true;
#endif
}

void pDirIteratorClose(PuliDirIterator** it) {
	if (it != NULL && *it != NULL) {
#ifdef _WIN32
#else
		closedir((*it)->dir);
#endif
		free(*it);
		*it = NULL;
	}
}
