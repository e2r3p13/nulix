#include <kernel/string.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x)\
test_count += 1;\
if(!(x)) {\
	printf("[\033[31mKO\033[0m]: %s l.%d\n", __func__, __LINE__);\
	failed_tests += 1;\
}

int test_count = 0;
int failed_tests = 0;

static int test_strlen() {
	const char s1[] = "hello world";
	const char s2[] = "";
	const char s3[] = {'\0'};
	const char s4[] = "0.0";

	ASSERT(strlen(s1) == 11);
	ASSERT(strlen(s2) == 0);
	ASSERT(strlen(s3) == 0);
	ASSERT(strlen(s4) == 3);

	return 0;
}

static int test_strchr() {
	const char s1[] = "ABBA";

	ASSERT(strchr(s1, 'A') == s1);
	ASSERT(strchr(s1, 'B') == s1 + 1);
	ASSERT(strchr(s1, '\0') == s1 + 4);
	ASSERT(strchr(s1, 'C') == NULL);

	return 0;
}

static int test_strrchr() {
	const char s1[] = "ABBA";
	
	ASSERT(strrchr(s1, 'A') == s1 + 3);
	ASSERT(strrchr(s1, 'B') == s1 + 2);
	ASSERT(strrchr(s1, '\0') == s1 + 4);
	ASSERT(strrchr(s1, 'C') == NULL);

	return 0;
}

static int test_strcmp() {
	const char s1[] = "";
	const char s2[] = {'\0'};
	const char s3[10] = {0};
	const char s4[] = "Hey";
	const char s5[] = "Hey\nHow you doing?";
	const char s6[] = "Hey\0";

	/* Against itself */
	ASSERT(strcmp(s1, s1) == 0);
	ASSERT(strcmp(s2, s2) == 0);
	ASSERT(strcmp(s3, s3) == 0);
	ASSERT(strcmp(s4, s4) == 0);
	ASSERT(strcmp(s5, s5) == 0);
	ASSERT(strcmp(s6, s6) == 0);

	/* Empty strings */
	ASSERT(strcmp(s1, s2) == 0);
	ASSERT(strcmp(s1, s3) == 0);
	ASSERT(strcmp(s2, s1) == 0);
	ASSERT(strcmp(s2, s3) == 0);
	ASSERT(strcmp(s3, s1) == 0);
	ASSERT(strcmp(s3, s2) == 0);

	/* Substrings */
	ASSERT(strcmp(s4, s5) != 0);
	ASSERT(strcmp(s4, s5) != 0);
	ASSERT(strcmp(s5, s1) != 0);

	return 0;
}

static int test_strncmp() {
	const char s1[] = "";
	const char s2[] = "Hello";
	const char s3[] = "Hello world";
	const char s4[] = "Heppo world";

	ASSERT(strncmp(s1, s2, 0) == 0);
	ASSERT(strncmp(s2, s3, 5) == 0);
	ASSERT(strncmp(s2, s3, 6) != 0);
	ASSERT(strncmp(s3, s4, 2) == 0);
	ASSERT(strncmp(s3, s4, 9) != 0);
	ASSERT(strncmp(s1, s4, 0) == 0); 
}

int main() {
	printf("-- Running test suite --\n");

	test_strlen();
	test_strchr();
	test_strrchr();
	test_strncmp();
	test_strcmp();

	if (failed_tests == 0) {
		printf("-- All %d tests passed --\n", test_count);
	} else {
		printf("-- %d/%d tests passed --\n", test_count - failed_tests, test_count);
	}
	printf("\033[33mWarning: Only 5 out of 20 functions are currently tested\033[0m\n");
	return 0;
}
