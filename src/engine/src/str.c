#include "boom/str.h"

char* StrAppend(char* trg, const size_t maxBufSize, const char* src) {
	return StrAppendN(trg, maxBufSize, src, StrLen(src));
}
char* StrAppendN(char* trg, const size_t maxBufSize, const char* src, const size_t srcLen) {
	const size_t trgLen = StrLen(trg);
	if (trgLen == 0) {
		return StrCpyN(trg, maxBufSize, src, srcLen);
	}

	if (srcLen + trgLen > maxBufSize) {
		return strncat(trg, src, maxBufSize - trgLen);
	}
	return strncat(trg, src, srcLen);
}
char* StrAppendChar(char* trg, size_t maxBufSize, char c) {
	char src[2] = { c, '\0' };
	return StrAppend(trg, maxBufSize, src);
}
char* StrCpy(char* trg, const size_t maxBufSize, const char* src) {
	return StrCpyN(trg, maxBufSize, src, StrLen(src));
}
char* StrCpyN(char* trg, const size_t maxBufSize, const char* src, const size_t srcLen) {
	if (srcLen > maxBufSize) {
		return strncpy(trg, src, maxBufSize);
	}
	return strncpy(trg, src, srcLen);
}
char* StrDup(const char* src) {
	char* p = (char*)malloc(StrLen(src) + 1);
	if (p != NULL) {
		strcpy(p, src);
	}
	return p;
}
const char* StrGetTok(char* trg, size_t maxBufSize, const char* src, const char delim) {
	if (StrLen(src) == 0) {
		return NULL;
	}

	const ssize_t idx = StrFindLastChar(src, delim);
	if (idx < 0) {
		StrCpy(trg, maxBufSize, src);
		return src + strlen(src);
	}
	StrCpyN(trg, maxBufSize, src, idx);
	return src + idx + 1;
}

ssize_t StrFindChar(const char* cstr, const char c) {
	char* p = strchr(cstr, c);
	if (p == NULL) {
		return -1;
	}
	return p - cstr;
}
ssize_t StrFindStr(const char* cstr, const char* toFind) {
	char* p = strstr(cstr, toFind);
	if (p == NULL) {
		return -1;
	}
	return p - cstr;
}
ssize_t StrFindLastChar(const char* cstr, char c) {
	char* p = strrchr(cstr, c);
	if (p == NULL) {
		return -1;
	}
	return p - cstr;
}
ssize_t StrFindLastStr(const char* cstr, const char* toFind) {
	const size_t toLen = StrLen(toFind);
	for (ssize_t i = (ssize_t)StrLen(cstr) - 1; i >= 0; i -= (ssize_t)toLen) {
		if (StrEqualsN(cstr + i, toFind, toLen)) {
			return i;
		}
	}
	return -1;
}
ssize_t StrFindAnyOfStr(const char* cstr, const char* chars) {
	const size_t len = StrLen(cstr);
	if (len == 0) return -1;
	for (ssize_t i = 0; i < len; i++) {
		const ssize_t pos = StrFindChar(chars, cstr[i]);
		if (pos != -1) {
			return pos;
		}
	}
	return -1;
}
ssize_t StrFindAnyLastOfStr(const char* cstr, const char* chars) {
	const size_t len = StrLen(cstr);
	if (len == 0) return -1;
	for (ssize_t i = (ssize_t)len - 1; i >= 0; i--) {
		const ssize_t pos = StrFindChar(chars, cstr[i]);
		if (pos != -1) {
			return pos;
		}
	}
	return -1;
}
ssize_t StrFindAnyNotOfStr(const char* cstr, const char* chars) {
	const size_t len = StrLen(cstr);
	if (len == 0) return -1;
	for (ssize_t i = 0; i < len; i++) {
		const ssize_t pos = StrFindChar(chars, cstr[i]);
		if (pos == -1) {
			return i;
		}
	}
	return -1;
}
ssize_t StrFindAnyLastNotOfStr(const char* cstr, const char* chars) {
	const size_t len = StrLen(cstr);
	if (len == 0) return -1;
	for (ssize_t i = (ssize_t)len - 1; i >= 0; i--) {
		const ssize_t pos = StrFindChar(chars, cstr[i]);
		if (pos == -1) {
			return i;
		}
	}
	return -1;
}
char* StrTrimStart(char* trg, size_t maxBufSize, const char* src, const char* chars) {
	const ssize_t idx = StrFindAnyNotOfStr(src, chars);
	if (idx == -1) {
		return NULL;
	}
	return StrCpy(trg + idx, maxBufSize, src);
}
char* StrTrimEnd(char* trg, size_t maxBufSize, const char* src, const char* chars) {
	const ssize_t idx = StrFindAnyLastNotOfStr(src, chars);
	if (idx == -1) {
		return NULL;
	}
	return StrCpyN(trg, maxBufSize, src, idx + 1);
}
char* StrTrim(char* trg, size_t maxBufSize, const char* src, const char* chars) {
	const ssize_t start = StrFindAnyNotOfStr(src, chars);
	if (start == -1) {
		return NULL;
	}
	const ssize_t end = StrFindAnyLastNotOfStr(src + start, chars);
	if (end == -1) {
		return NULL;
	}

	return StrCpyN(trg + start, maxBufSize, src, end + 1);
}

bool StrToInt(const char* cstr, int* i) {
	if (StrStartWithStr(cstr, "0x") || StrStartWithStr(cstr, "0X")) {
		return StrToIntRadix(cstr + 2, i, 16);
	} else if (StrStartWithChar(cstr, '#')) {
		return StrToIntRadix(cstr + 1, i, 16);
	} else if (StrStartWithStr(cstr, "0o") || StrStartWithStr(cstr, "0O")) {
		return StrToIntRadix(cstr + 2, i, 8);
	} else if (StrStartWithChar(cstr, '0')) {
		return StrToIntRadix(cstr, i, 8);
	} else if (StrStartWithStr(cstr, "0b") || StrStartWithStr(cstr, "0B")) {
		return StrToIntRadix(cstr, i, 2);
	} else {
		return StrToIntRadix(cstr, i, 10);
	}
}
bool StrToIntRadix(const char* cstr, int* i, int radix) {
	errno = 0;
	long value = strtol(cstr, NULL, radix);
	if (errno != 0) {
		return false;
	}
	*i = (int)value;
	return true;
}
bool StrToUInt(const char* cstr, unsigned* i) {
	if (StrStartWithStr(cstr, "0x") || StrStartWithStr(cstr, "0X")) {
		return StrToUIntRadix(cstr + 2, i, 16);
	} else if (StrStartWithChar(cstr, '#')) {
		return StrToUIntRadix(cstr + 1, i, 16);
	} else if (StrStartWithStr(cstr, "0o") || StrStartWithStr(cstr, "0O")) {
		return StrToUIntRadix(cstr + 2, i, 8);
	} else if (StrStartWithChar(cstr, '0')) {
		return StrToUIntRadix(cstr, i, 8);
	} else if (StrStartWithStr(cstr, "0b") || StrStartWithStr(cstr, "0B")) {
		return StrToUIntRadix(cstr, i, 2);
	} else {
		return StrToUIntRadix(cstr, i, 10);
	}
}
bool StrToUIntRadix(const char* cstr, unsigned* i, int radix) {
	errno = 0;
	unsigned long value = strtoul(cstr, NULL, radix);
	if (errno != 0) {
		return false;
	}
	*i = (unsigned)value;
	return true;
}
bool StrToBool(const char* cstr, bool* b) {
	bool result;
	if (StrEqualsNoCase(cstr, "true")) {
		result = true;
	} else if (StrEqualsNoCase(cstr, "false")) {
		result = false;
	} else {
		int i;
		if (!StrToInt(cstr, &i)) {
			return false;
		}
		result = i != 0;
	}
	*b = result;
	return true;
}
bool StrToFloat(const char* cstr, float* f) {
	errno = 0;
	float value = strtof(cstr, NULL);
	if (errno != 0) {
		return false;
	}
	*f = value;
	return true;
}
bool StrToDouble(const char* cstr, double* d) {
	errno = 0;
	double value = strtod(cstr, NULL);
	if (errno != 0) {
		return false;
	}
	*d = value;
	return true;
}

bool CharIsWhiteSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
bool CharIsUpperCase(char c) {
	return c >= 'A' && c <= 'Z';
}
bool CharIsLowerCase(char c) {
	return c >= 'a' && c <= 'z';
}
bool CharIsLetter(char c) {
	return CharIsLowerCase(c) || CharIsUpperCase(c);
}
bool CharIsNumber(char c) {
	return c >= '0' && c <= '9';
}
bool CharIsAlphaNum(char c) {
	return CharIsLetter(c) || CharIsNumber(c);
}
char CharToUpperCase(char c) {
	return (char)(CharIsLowerCase(c) ? ((c - 'a') + 'A') : c);
}
char CharToLowerCase(char c) {
	return (char)(CharIsLowerCase(c) ? ((c - 'A') + 'a') : c);
}

bool StrStartWithChar(const char* cstr, const char c) {
	return StrLen(cstr) > 0 && cstr[0] == c;
}
bool StrStartWithStr(const char* cstr, const char* prefix) {
	const size_t size = StrLen(cstr);
	const size_t prefixSize = StrLen(prefix);
	return size >= prefixSize && StrEqualsN(cstr, prefix, prefixSize);
}
bool StrEndWithChar(const char* cstr, const char c) {
	const size_t size = StrLen(cstr);
	return size > 0 && cstr[size - 1] == c;
}
bool StrEndWithStr(const char* cstr, const char* suffix) {
	const size_t size = StrLen(cstr);
	const size_t prefixSize = StrLen(suffix);
	return size >= prefixSize && StrEqualsN(cstr + prefixSize - size, suffix, prefixSize);
}

bool WStrStartWithStr(const wchar_t* wcs, const wchar_t* prefix) {
	return wcs != NULL && prefix != NULL && WStrEqualsN(wcs, prefix, wcslen(prefix));
}
bool WStrStartWithChar(const wchar_t* wcs, wchar_t c) {
	return wcs != NULL && wcs[0] == c;
}
bool WStrEndWithStr(const wchar_t* wcs, const wchar_t* suffix) {
	const size_t size = WStrLen(wcs);
	const size_t prefixSize = WStrLen(suffix);
	return wcs != NULL && size >= prefixSize && WStrEqualsN(wcs + prefixSize - size, suffix, prefixSize);
}
bool WStrEndWithChar(const wchar_t* wcs, wchar_t c) {
	const size_t size = WStrLen(wcs);
	return wcs != NULL && size > 0 && wcs[size - 1] == c;
}
wchar_t* WStrAppend(wchar_t* trg, size_t maxBufSize, const wchar_t* src) {
	const size_t srcLen = wcslen(src);
	const size_t trgLen = wcslen(trg);
	if (trgLen == 0) {
		return wcscpy_s(trg, maxBufSize, src) == 0 ? trg : NULL;
	}

	if (srcLen + trgLen > maxBufSize) {
		return wcsncat(trg, src, maxBufSize - trgLen);
	}
	return wcsncat(trg, src, srcLen);
}
wchar_t* WStrAppendChar(wchar_t* trg, size_t maxBufSize, wchar_t c) {
	wchar_t src[2] = { c, L'\0' };
	return WStrAppend(trg, maxBufSize, src);
}

bool WStrToInt(const wchar_t* wcs, int* i) {
	if (WStrStartWithStr(wcs, L"0x") || WStrStartWithStr(wcs, L"0X")) {
		return WStrToIntRadix(wcs + 2, i, 16);
	} else if (WStrStartWithChar(wcs, L'#')) {
		return WStrToIntRadix(wcs + 1, i, 16);
	} else if (WStrStartWithStr(wcs, L"0o") || WStrStartWithStr(wcs, L"0O")) {
		return WStrToIntRadix(wcs + 2, i, 8);
	} else if (WStrStartWithChar(wcs, '0')) {
		return WStrToIntRadix(wcs, i, 8);
	} else if (WStrStartWithStr(wcs, L"0b") || WStrStartWithStr(wcs, L"0B")) {
		return WStrToIntRadix(wcs, i, 2);
	} else {
		return WStrToIntRadix(wcs, i, 10);
	}
}
bool WStrToIntRadix(const wchar_t* wcs, int* i, int radix) {
	errno = 0;
	long value = wcstol(wcs, NULL, radix);
	if (errno != 0) {
		return false;
	}
	*i = (int)value;
	return true;
}
bool WStrToUInt(const wchar_t* wcs, unsigned* i) {
	if (WStrStartWithStr(wcs, L"0x") || WStrStartWithStr(wcs, L"0X")) {
		return WStrToUIntRadix(wcs + 2, i, 16);
	} else if (WStrStartWithChar(wcs, L'#')) {
		return WStrToUIntRadix(wcs + 1, i, 16);
	} else if (WStrStartWithStr(wcs, L"0o") || WStrStartWithStr(wcs, L"0O")) {
		return WStrToUIntRadix(wcs + 2, i, 8);
	} else if (WStrStartWithChar(wcs, '0')) {
		return WStrToUIntRadix(wcs, i, 8);
	} else if (WStrStartWithStr(wcs, L"0b") || WStrStartWithStr(wcs, L"0B")) {
		return WStrToUIntRadix(wcs, i, 2);
	} else {
		return WStrToUIntRadix(wcs, i, 10);
	}
}
bool WStrToUIntRadix(const wchar_t* wcs, unsigned* i, int radix) {
	errno = 0;
	unsigned long value = wcstoul(wcs, NULL, radix);
	if (errno != 0) {
		return false;
	}
	*i = (unsigned)value;
	return true;
}
bool WStrToBool(const wchar_t* wcs, bool* b) {
	bool result;
	if (WStrEqualsNoCase(wcs, L"true")) {
		result = true;
	} else if (WStrEqualsNoCase(wcs, L"false")) {
		result = false;
	} else {
		int i;
		if (!WStrToInt(wcs, &i)) {
			return false;
		}
		result = i != 0;
	}
	*b = result;
	return true;
}
bool WStrToFloat(const wchar_t* wcs, float* f) {
	errno = 0;
	float value = wcstof(wcs, NULL);
	if (errno != 0) {
		return false;
	}
	*f = value;
	return true;
}
bool WStrToDouble(const wchar_t* wcs, double* d) {
	errno = 0;
	double value = wcstod(wcs, NULL);
	if (errno != 0) {
		return false;
	}
	*d = value;
	return true;
}