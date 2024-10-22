#include "ut.h"
#include "puli.h"

int main() {
	UT_BEGIN("puli")
		UT_BEGIN_TEST("puli_str - char utils")
			for (int c = 'a'; c < 'z'; c++) { UT_IS_TRUE(pCharIsLetter(c)); }
			for (int c = 'A'; c < 'Z'; c++) { UT_IS_TRUE(pCharIsLetter(c)); }
			for (int c = '0'; c < '9'; c++) { UT_IS_FALSE(pCharIsLetter(c)); }
			for (int c = '0'; c < '9'; c++) { UT_IS_TRUE(pCharIsNumber(c)); }
			for (int c = '0'; c < '9'; c++) { UT_IS_TRUE(pCharIsNumberHex(c)); }
			for (int c = 'a'; c < 'f'; c++) { UT_IS_TRUE(pCharIsNumberHex(c)); }
			for (int c = 'A'; c < 'F'; c++) { UT_IS_TRUE(pCharIsNumberHex(c)); }
			for (int c = 'a'; c < 'z'; c++) { UT_IS_TRUE(pCharIsAlphaNum(c)); }
			for (int c = 'A'; c < 'Z'; c++) { UT_IS_TRUE(pCharIsAlphaNum(c)); }
			for (int c = '0'; c < '9'; c++) { UT_IS_TRUE(pCharIsAlphaNum(c)); }

			UT_IS_TRUE(pCharIsSpace(' '));
			UT_IS_TRUE(pCharIsSpace('\t'));
			UT_IS_TRUE(pCharIsSpace('\r'));
			UT_IS_TRUE(pCharIsSpace('\n'));
			UT_IS_FALSE(pCharIsSpace('T'));
		UT_END_TEST
		UT_BEGIN_TEST("puli_str - string concatenation")
			char a[64] = "Hello ";
			char b[64] = "world";
			pStrCat(a, 64, b);
			UT_IS_TRUE(pStrEquals(a, "Hello world"));
		UT_END_TEST
	UT_END

	return 0;
}