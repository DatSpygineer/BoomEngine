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

PuliError pStrCat(char* dest, size_t dest_max, const char* src);
PuliError pStrnCat(char* dest, size_t dest_max, const char* src, size_t count);
PuliError pStrCatFormat(char* dest, size_t dest_max, const char* format, ...);
PuliError pStrCatFormatV(char* dest, size_t dest_max, const char* format, va_list args);
PuliError pStrCpy(char* dest, size_t dest_max, const char* src);
PuliError pStrnCpy(char* dest, size_t dest_max, const char* src, size_t count);

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

size_t pStrHash(const char* str);

#define pStrLen(__str) ((__str) == NULL ? 0 : strlen((__str)))

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

size_t pStrSplit(const char* str, char** out, size_t tokenMaxCount, size_t tokenMaxBufSize, char delim);

bool pStrToInt(const char* str, int* value);
bool pStrToIntRadix(const char* str, int* value, int radix);
bool pStrToUInt(const char* str, unsigned int* value);
bool pStrToUIntRadix(const char* str, unsigned int* value, int radix);
bool pStrToFloat(const char* str, float* value);
bool pStrToDouble(const char* str, double* value);
bool pStrToBool(const char* str, bool* value);

#ifdef __cplusplus
}
#endif