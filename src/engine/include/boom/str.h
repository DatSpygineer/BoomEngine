#pragma once

#include "defs.h"

BOOM_HEADER_START

#include <limits.h>

#define StrEquals(__lhs, __rhs) (strcmp(__lhs, __rhs) == 0)
#define StrEqualsN(__lhs, __rhs, __n) (strncmp(__lhs, __rhs, __n) == 0)
#ifdef _WIN32
	#define StrEqualsNoCase(__lhs, __rhs) (_stricmp(__lhs, __rhs) == 0)
	#define StrEqualsNoCaseN(__lhs, __rhs, __n) (_strnicmp(__lhs, __rhs, __n) == 0)
#else
	#define StrEqualsNoCase(__lhs, __rhs) (strcasecmp(__lhs, __rhs) == 0)
	#define StrEqualsNoCaseN(__lhs, __rhs, __n) (strncasecmp(__lhs, __rhs, __n) == 0)
#endif

#define BOOM_PATH_SHORT_MAX 64

#define StrLen(__str) ((__str) != NULL ? strlen(__str) : 0)

BOOM_API bool CharIsWhiteSpace(char c);
BOOM_API bool CharIsUpperCase(char c);
BOOM_API bool CharIsLowerCase(char c);
BOOM_API bool CharIsLetter(char c);
BOOM_API bool CharIsNumber(char c);
BOOM_API bool CharIsAlphaNum(char c);
BOOM_API char CharToUpperCase(char c);
BOOM_API char CharToLowerCase(char c);

BOOM_API bool StrStartWithChar(const char* cstr, char c);
BOOM_API bool StrStartWithStr(const char* cstr, const char* prefix);
BOOM_API bool StrEndWithChar(const char* cstr, char c);
BOOM_API bool StrEndWithStr(const char* cstr, const char* suffix);

BOOM_API char* StrAppend(char* trg, size_t maxBufSize, const char* src);
BOOM_API char* StrAppendN(char* trg, size_t maxBufSize, const char* src, size_t srcLen);
BOOM_API char* StrAppendChar(char* trg, size_t maxBufSize, char c);
BOOM_API char* StrCpy(char* trg, size_t maxBufSize, const char* src);
BOOM_API char* StrCpyN(char* trg, size_t maxBufSize, const char* src, size_t srcLen);
BOOM_API char* StrDup(const char* src);

BOOM_API const char* StrGetTok(char* trg, size_t maxBufSize, const char* src, char delim);

BOOM_API ssize_t StrFindChar(const char* cstr, char c);
BOOM_API ssize_t StrFindStr(const char* cstr, const char* toFind);
BOOM_API ssize_t StrFindLastChar(const char* cstr, char c);
BOOM_API ssize_t StrFindLastStr(const char* cstr, const char* toFind);
BOOM_API ssize_t StrFindAnyOfStr(const char* cstr, const char* chars);
BOOM_API ssize_t StrFindAnyLastOfStr(const char* cstr, const char* chars);
BOOM_API ssize_t StrFindAnyNotOfStr(const char* cstr, const char* chars);
BOOM_API ssize_t StrFindAnyLastNotOfStr(const char* cstr, const char* chars);

BOOM_API char* StrTrimStart(char* trg, size_t maxBufSize, const char* src, const char* chars);
BOOM_API char* StrTrimEnd(char* trg, size_t maxBufSize, const char* src, const char* chars);
BOOM_API char* StrTrim(char* trg, size_t maxBufSize, const char* src, const char* chars);

BOOM_API bool StrToInt(const char* cstr, int* i);
BOOM_API bool StrToIntRadix(const char* cstr, int* i, int radix);
BOOM_API bool StrToUInt(const char* cstr, unsigned* i);
BOOM_API bool StrToUIntRadix(const char* cstr, unsigned* i, int radix);
BOOM_API bool StrToBool(const char* cstr, bool* b);
BOOM_API bool StrToFloat(const char* cstr, float* f);
BOOM_API bool StrToDouble(const char* cstr, double* d);


#define WStrEquals(__lhs, __rhs) (wcscmp(__lhs, __rhs) == 0)
#define WStrEqualsN(__lhs, __rhs, __n) (wcsncmp(__lhs, __rhs, __n) == 0)
#ifdef _WIN32
	#define WStrEquals(__lhs, __rhs) (wcscmp(__lhs, __rhs) == 0)
	#define WStrEqualsN(__lhs, __rhs, __n) (wcsncmp(__lhs, __rhs, __n) == 0)
	#define WStrEqualsNoCase(__lhs, __rhs) (_wcsicmp(__lhs, __rhs) == 0)
	#define WStrEqualsNoCaseN(__lhs, __rhs, __n) (_wcsnicmp(__lhs, __rhs, __n) == 0)
	#define WBOOM_PATH_SEPARATOR L'\\'
	#define TBOOM_PATH_SEPARATOR WBOOM_PATH_SEPARATOR
#else
	#define WStrEqualsNoCase(__lhs, __rhs) (wcscasecmp(__lhs, __rhs) == 0)
	#define WStrEqualsNoCaseN(__lhs, __rhs, __n) (wcsncasecmp(__lhs, __rhs, __n) == 0)
	#define WBOOM_PATH_SEPARATOR L'/'
	#define TBOOM_PATH_SEPARATOR BOOM_PATH_SEPARATOR
#endif

#define WStrLen(__str) ((__str) != NULL ? wcslen(__str) : 0)

BOOM_API bool WStrStartWithStr(const wchar_t* wcs, const wchar_t* prefix);
BOOM_API bool WStrStartWithChar(const wchar_t* wcs, wchar_t c);
BOOM_API bool WStrEndWithStr(const wchar_t* wcs, const wchar_t* suffix);
BOOM_API bool WStrEndWithChar(const wchar_t* wcs, wchar_t c);
BOOM_API wchar_t* WStrAppend(wchar_t* trg, size_t maxBufSize, const wchar_t* src);
BOOM_API wchar_t* WStrAppendChar(wchar_t* trg, size_t maxBufSize, wchar_t c);

BOOM_API bool WStrToInt(const wchar_t* wcs, int* i);
BOOM_API bool WStrToIntRadix(const wchar_t* wcs, int* i, int radix);
BOOM_API bool WStrToUInt(const wchar_t* wcs, unsigned* i);
BOOM_API bool WStrToUIntRadix(const wchar_t* wcs, unsigned* i, int radix);
BOOM_API bool WStrToBool(const wchar_t* wcs, bool* b);
BOOM_API bool WStrToFloat(const wchar_t* wcs, float* f);
BOOM_API bool WStrToDouble(const wchar_t* wcs, double* d);

#ifdef _WIN32
	#define TStrEquals WStrEquals
	#define TStrEqualsN WStrEqualsN
	#define TStrEqualsNoCase WStrEqualsNoCase
	#define TStrEqualsNoCaseN WStrEqualsNoCaseN
	#define TStrLen WStrLen
	#define TStrStartWithStr WStrStartWithStr
	#define TStrStartWithChar WStrStartWithChar
	#define TStrEndWithStr WStrEndWithStr
	#define TStrEndWithChar WStrEndWithChar
	#define TStrAppend WStrAppend
	#define TStrAppendChar WStrAppendChar
	#define TStrToInt WStrToInt
	#define TStrToIntRadix WStrToIntRadix
	#define TStrToUInt WStrToUInt
	#define TStrToUIntRadix WStrToUIntRadix
	#define TStrToBool WStrToBool
	#define TStrToFloat WStrToFloat
	#define TStrToDouble WStrToDouble

	#define TStr(__x) L##__x
	#define TChar wchar_t
#else
	#define TStrEquals StrEquals
	#define TStrEqualsN StrEqualsN
	#define TStrEqualsNoCase StrEqualsNoCase
	#define TStrEqualsNoCaseN StrEqualsNoCaseN
	#define TStrLen StrLen
	#define TStrStartWithStr StrStartWithStr
	#define TStrStartWithChar StrStartWithChar
	#define TStrEndWithStr StrEndWithStr
	#define TStrEndWithChar StrEndWithChar
	#define TStrAppend StrAppend
	#define TStrAppendChar StrAppendChar
	#define TStrToInt StrToInt
	#define TStrToIntRadix StrToIntRadix
	#define TStrToUInt StrToUInt
	#define TStrToUIntRadix StrToUIntRadix
	#define TStrToBool StrToBool
	#define TStrToFloat StrToFloat
	#define TStrToDouble StrToDouble

	#define TStr(__x) __x
	#define TChar char
#endif

BOOM_HEADER_END