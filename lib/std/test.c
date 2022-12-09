
#include <stdio.h>
#include <kernel/stdlib.h>

#define TEST(cmp) (cmp) ? "\x1b[32mOK\x1b[0m" : "\x1b[31mKO\x1b[0m"

#define TEST_STRUCT2(name, type_a, type_b) struct test_##name {type_a a; type_b b;}
#define TEST_STRUCT3(name, type_a, type_b, type_c) struct test_##name {type_a a; type_b b; type_c c;}
#define TEST_STRUCT4(name, type_a, type_b, type_c, type_d) struct test_##name {type_a a; type_b b; type_c c; type_d d;}

#ifndef ULONG_MAX
#define ULONG_MAX	((unsigned long)(~0L))		// 0xFFFFFFFF
#endif

#ifndef LONG_MAX
#define LONG_MAX	((long)(ULONG_MAX >> 1))	// 0x7FFFFFFF
#endif

#ifndef LONG_MIN
#define LONG_MIN	((long)(~LONG_MAX))			// 0x80000000
#endif

TEST_STRUCT2(abs, int, int);
TEST_STRUCT4(strtol, char *, int, char, int);
TEST_STRUCT2(atoi, char *, int);

int main() {

	printf("abs\n");
	struct test_abs t_abs[] = {
		{1, 1},
		{23656666, 23656666},
		{-1457459, 1457459},
		{-1, 1},
		{999482, 999482},
		{-123145564, 123145564},
		{0, 0}
	};
	printf("TEST:\n");
	for (int i = 0; i < sizeof(t_abs) / sizeof(struct test_abs); i++) {
		printf("%d: %-10d\t[ %s ]\n", i, t_abs[i].a, TEST(abs(t_abs[i].a) == t_abs[i].b));
	}
	printf("END\n\n");

	printf("strtol\n");
	struct test_strtol t_strtol[] = {
		{"1aze", 1, 'a', 10},
		{"23656666", 23656666, 0, 10},
		{"-1457459", -1457459, 0, 10},
		{"-1", -1, 0, 10},
		{"999482", 999482, 0, 10},
		{"-123145564", -123145564, 0, 10},
		{"0x100", 0, 'x', 10},
		{"0x10", 16, 0, 16},
		{"10", 10, 0, 10},
		{"10", 5, 0, 5},
		{"10", 8, 0, 8},
		{"0", 0, 0, 10},
	};
	char *ptr;
	printf("TEST:\n");
	for (int i = 0; i < sizeof(t_strtol) / sizeof(struct test_strtol); i++) {
		long tmp = strtol(t_strtol[i].a, &ptr, t_strtol[i].d);
		printf("%02d: %15s (%2d)\t%-15lx[ %s ]\n", i, t_strtol[i].a, t_strtol[i].d, tmp,TEST(tmp == t_strtol[i].b && *ptr == t_strtol[i].c));
	}
	printf("END\n\n");

	printf("atoi\n");
	struct test_atoi t_atoi[] = {
		{"1", 1},
		{"23656666", 23656666},
		{"-1457459", -1457459},
		{"-1", -1},
		{"999482", 999482},
		{"-123145564", -123145564},
		{"0", 0}
	};
	printf("TEST:\n");
	for (int i = 0; i < sizeof(t_atoi) / sizeof(struct test_atoi); i++) {
		printf("%d: %-10s\t[ %s ]\n", i, t_atoi[i].a, TEST(atoi(t_atoi[i].a) == t_atoi[i].b));
	}
	printf("END\n\n");

	printf("UL: %lx LMAX: %lx LMIN: %lx\n", ULONG_MAX, LONG_MAX, LONG_MIN);

}
