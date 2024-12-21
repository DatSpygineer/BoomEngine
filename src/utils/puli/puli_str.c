#include "puli_str.h"

#include <puli_fsys.h>
#include <stdarg.h>
#include <errno.h>

bool pCharIsSpace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
bool pCharIsLetter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool pCharIsNumber(char c) {
	return c >= '0' && c <= '9';
}
bool pCharIsNumberHex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
bool pCharIsAlphaNum(char c) {
	return pCharIsLetter(c) || pCharIsNumber(c);
}

char* pStrDup(const char* str) {
	char* p = (char*)malloc(strlen(str) + 1);
	if (p == NULL) return NULL;
	pStrCpy(p, strlen(str) + 1, str);
	p[strlen(str)] = '\0';
	return p;
}

PuliError pStrCat(char* dest, const size_t dest_max, const char* src) {
	return pStrnCat(dest, dest_max, src, strlen(src));
}
PuliError pStrnCat(char* dest, const size_t dest_max, const char* src, const size_t count) {
	errno = 0;
	const size_t src_len = strlen(src);
	const size_t dest_len = strlen(dest);
	if (dest_len + src_len < dest_max) {
		ssize_t remain = (ssize_t)dest_max - (ssize_t)dest_len;
		if (remain > 0) {
			strncat(dest, src, remain);
		} else {
			return P_ERROR_BUFFER_IS_FULL;
		}
	} else {
		strncat(dest, src, count);
	}

	return errno;
}
PuliError pStrCatFormat(char* dest, size_t dest_max, const char* format, ...) {
	va_list args;
	va_start(args, format);
	const PuliError e = pStrCatFormatV(dest, dest_max, format, args);
	va_end(args);
	return e;
}
PuliError pStrCatFormatV(char* dest, size_t dest_max, const char* format, va_list args) {
	errno = 0;
	char buf[P_PATH_MAX];
	vsnprintf(buf, P_PATH_MAX, format, args);
	if (errno != 0) {
		return errno;
	}
	return pStrCat(dest, dest_max, buf);
}
PuliError pStrCpy(char* dest, const size_t dest_max, const char* src) {
	return pStrnCpy(dest, dest_max, src, strlen(src));
}
PuliError pStrnCpy(char* dest, const size_t dest_max, const char* src, const size_t count) {
	errno = 0;
	const size_t copy_len = dest_max <= count ? dest_max : count;

	if (copy_len > 0) {
		strncpy(dest, src, copy_len);
	} else {
		return P_ERROR_BUFFER_IS_FULL;
	}

	return errno;
}

bool pStrStartsWith(const char* str, char prefix) {
	return str != NULL && str[0] == prefix;
}
bool pStrStartsWithStr(const char* str, const char* prefix) {
	const size_t len = strlen(prefix);
	return str != NULL && prefix != NULL && len > 0 && pStrnEquals(str, prefix, len);
}
bool pStrEndsWith(const char* str, char suffix) {
	const size_t len = strlen(str);
	return len > 0 && str[len - 1] == suffix;
}
bool pStrEndsWithStr(const char* str, const char* suffix) {
	const size_t len = strlen(str);
	const size_t len_suffix = strlen(suffix);

	return pStrnEquals(str + len - len_suffix, suffix, len_suffix);
}

ssize_t pStrFind(const char* str, char c) {
	char* p = strchr(str, c);
	return p == NULL ? -1 : p - str;
}
ssize_t pStrFindLast(const char* str, char c) {
	char* p = strrchr(str, c);
	return p == NULL ? -1 : p - str;
}
ssize_t pStrFindStr(const char* str, const char* to_find) {
	char* p = strstr(str, to_find);
	return p == NULL ? -1 : p - str;
}
ssize_t pStrFindStrLast(const char* str, const char* to_find) {
	const ssize_t str_len     = (ssize_t)strlen(str);
	const ssize_t to_find_len = (ssize_t)strlen(to_find);
	for (ssize_t i = str_len - to_find_len; i >= 0; i -= to_find_len) {
		if (strncmp(str, to_find, to_find_len) == 0) {
			return i;
		}
	}
	return -1;
}
ssize_t pStrFindAny(const char* str, const char* to_find) {
	const ssize_t span = (ssize_t)strcspn(str, to_find);
	if (span >= strlen(str)) {
		return -1;
	}
	return span;
}
ssize_t pStrFindAnyLast(const char* str, const char* to_find) {
	for (ssize_t i = (ssize_t)strlen(str) - 1; i >= 0; i--) {
		if (pStrFind(to_find, str[i]) >= 0) {
			return i;
		}
	}
	return -1;
}
ssize_t pStrFindAnyNot(const char* str, const char* to_find) {
	const ssize_t span = (ssize_t)strspn(str, to_find);
	if (span >= strlen(str)) {
		return -1;
	}
	return span;
}
ssize_t pStrFindAnyLastNot(const char* str, const char* to_find) {
	for (ssize_t i = (ssize_t)strlen(str) - 1; i >= 0; i--) {
		if (pStrFind(to_find, str[i]) < 0) {
			return i;
		}
	}
	return -1;
}

size_t pStrHash(const char* str) {
	size_t hash = 0;
	char c;
	while ((c = *str++) != '\0') {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

const char* pStrGetTok(const char* str, char* line, size_t maxBufSize, char delim) {
	if (str == NULL || strlen(str) == 0 || line == NULL) return NULL;

	size_t write_i = 0;
	while (*str != delim && *str != '\0' && write_i < maxBufSize) {
		line[write_i++] = *str++;
	}
	line[write_i] = '\0';
	return str;
}

size_t pStrSplit(const char* str, char** out, size_t tokenMaxCount, size_t tokenMaxBufSize, char delim) {
	size_t i = 0;
	while (pStrGetTok(str, out[i], tokenMaxBufSize, delim) && i < tokenMaxCount) {
		i++;
	}
	return i;
}


bool pStrToInt(const char* str, int* value) {
	return pStrToIntRadix(str, value, 10);
}
bool pStrToIntRadix(const char* str, int* value, int radix) {
	errno = 0;
	long i = strtol(str, NULL, radix);
	if (errno == 0) {
		return false;
	}
	*value = (int)i;
	return true;
}
bool pStrToUInt(const char* str, unsigned int* value) {
	return pStrToUIntRadix(str, value, 10);
}
bool pStrToUIntRadix(const char* str, unsigned int* value, int radix) {
	errno = 0;
	unsigned long i = strtoul(str, NULL, radix);
	if (errno == 0) {
		return false;
	}
	*value = (unsigned int)i;
	return true;
}
bool pStrToFloat(const char* str, float* value) {
	errno = 0;
	const float f = strtof(str, NULL);
	if (errno == 0) {
		return false;
	}
	*value = f;
	return true;
}
bool pStrToDouble(const char* str, double* value)  {
	errno = 0;
	const double f = strtod(str, NULL);
	if (errno == 0) {
		return false;
	}
	*value = f;
	return true;
}
bool pStrToBool(const char* str, bool* value) {
	if (pStrEqualsNoCase(str, "true")) {
		*value = true;
		return true;
	}
	if (pStrEqualsNoCase(str, "false")) {
		*value = false;
		return true;
	}

	int ival = 0;
	if (pStrToInt(str, &ival)) {
		*value = ival != 0;
		return true;
	}
	return false;
}