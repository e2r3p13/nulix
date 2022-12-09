// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/strtoll.c
 *
 * Translate string to number.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#ifndef ULLONG_MAX
#define ULLONG_MAX	((unsigned long long)(~0LL))	// 0xFFFFFFFF_FFFFFFFF
#endif

#ifndef LLONG_MAX
#define LLONG_MAX	((long long)(ULLONG_MAX >> 1))	// 0x7FFFFFFF_FFFFFFFF
#endif

#ifndef LLONG_MIN
#define LLONG_MIN	((long long)(~LLONG_MAX))		// 0x80000000_00000000
#endif

/*
 * Convert a string to a long long integer.
 */
long long strtoll(const char *nptr, char **endptr, int base) {
	const char *s = nptr;
	int c;
	unsigned long long cutoff, acc;
	int neg = 0, cutlim, any;

	do {
		c = *s++;
	} while (c == ' ');

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+') {
		c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	cutoff = neg ? -(unsigned long long)LLONG_MIN : LLONG_MAX;
	cutlim = cutoff % (unsigned long long)base;
	cutoff /= (unsigned long long)base;

	for (acc = 0, any = 0;; c = *s++) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'Z') {
			c -= 'A' - 10;
		} else if (c >= 'a' && c <= 'z') {
			c -= 'a' - 10;
		} else {
			break;
		}
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0)
		acc = neg ? LLONG_MIN : LLONG_MAX;
	else if (neg)
		acc = -acc;

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return acc;
}
