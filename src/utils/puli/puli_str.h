#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "puli_defs.h"

bool pCharIsSpace(char c);
bool pCharIsLetter(char c);
bool pCharIsNumber(char c);
bool pCharIsNumberHex(char c);
bool pCharIsAlphaNum(char c);

char* pStrDup(const char* str);

errno_t pStrCat(char* dest, size_t dest_max, const char* src);
errno_t pStrnCat(char* dest, size_t dest_max, const char* src, size_t count);
errno_t pStrCatFormat(char* dest, size_t dest_max, const char* format, ...);
errno_t pStrCatFormatV(char* dest, size_t dest_max, const char* format, va_list args);
errno_t pStrCpy(char* dest, size_t dest_max, const char* src);
errno_t pStrnCpy(char* dest, size_t dest_max, const char* src, size_t count);

bool pStrStartsWith(const char* str, char prefix);
bool pStrStartsWithStr(const char* str, const char* prefix);
bool pStrEndsWith(const char* str, char suffix);
bool pStrEndsWithStr(const char* str, const char* suffix);

ssize_t pStrFind(const char* str, char c);
ssize_t pStrFindLast(const char* str, char c);
ssize_t pStrFindStr(const char* str, const char* to_find);
ssize_t pStrFindStrLast(const char* str, const char* to_find);
ssize_t pStrFindAny(const char* str, const char* to_find);
ssize_t pStrFindAnyLast(const char* str, const char* to_find);
ssize_t pStrFindAnyNot(const char* str, const char* to_find);
ssize_t pStrFindAnyLastNot(const char* str, const char* to_find);

#define pStrEquals(__lhs, __rhs)       (strcmp(__lhs, __rhs) == 0)
#define pStrnEquals(__lhs, __rhs, __n) (strncmp(__lhs, __rhs, __n) == 0)

#ifdef __GNUC__
	#define pStrEqualsNoCase(__lhs, __rhs) (strcasecmp(__lhs, __rhs) == 0)
	#define pStrnEqualsNoCase(__lhs, __rhs, __n) (strncasecmp(__lhs, __rhs, __n) == 0)
#else
	#define pStrEqualsNoCase(__lhs, __rhs) (stricmp(__lhs, __rhs) == 0)
	#define pStrnEqualsNoCase(__lhs, __rhs, __n) (strnicmp(__lhs, __rhs, __n) == 0)
#endif

const char* pStrGetTok(const char* str, char* line, size_t maxBufSize, char delim);

#ifdef __cplusplus
}
#endif