#include "ut.h"

#include <stdio.h>
#include <time.h>

static int test_count = 0;
static int test_pass_count = 0;
static int test_overall_count = 0;
static int test_overall_pass_count = 0;
static const char* test_name = "UNKNOWN";

void ut_begin(const char* name) {
	time_t t = time(NULL);

	puts("========================================");
	printf(" UT %s - %s\n", name, ctime(&t));
	puts("========================================");
}
void ut_begin_test(const char* name) {
	test_count++;
	test_name = name;
}
void ut_end_test(int case_num, int pass_num) {
	const char* label = "[FAIL]";
	if (case_num == pass_num) {
		test_pass_count++;
		test_overall_pass_count++;
		label = "[PASS]";
	}

	size_t len = printf(" %s ", test_name) - 7;
	for (size_t i = 0; i < len; i++) {
		putc('.', stdout);
	}
	printf(" %s (%d/%d)\n", label, pass_num, case_num);
	test_overall_count++;
}
void ut_end() {
	puts("========================================");
	printf(" Overall: %d/%d\n", test_overall_pass_count, test_overall_count);
	puts("========================================");
}