#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void ut_begin(const char* name);
void ut_begin_test(const char* name);
void ut_end_test(int case_num, int pass_num);
void ut_end();

#define UT_BEGIN(name) ut_begin(name);
#define UT_BEGIN_TEST(name) { ut_begin_test(name); int case_num = 0, pass_num = 0;

#define UT_IS_TRUE(__expression) case_num++; if (__expression) { pass_num++; }
#define UT_IS_FALSE(__expression) case_num++; if (!(__expression)) { pass_num++; }
#define UT_IS_EQUALS(__lhs, __rhs) UT_IS_TRUE((__lhs) == (__rhs))
#define UT_IS_NOT_EQUALS(__lhs, __rhs) UT_IS_TRUE((__lhs) != (__rhs))

#define UT_END_TEST ut_end_test(case_num, pass_num); }
#define UT_END ut_end();

#ifdef __cplusplus
}
#endif