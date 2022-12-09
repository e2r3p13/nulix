// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/strtol.c
 *
 * Translate string to number.
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#ifndef ULONG_MAX
#define ULONG_MAX	((unsigned long)(~0L))		// 0xFFFFFFFF
#endif

#ifndef LONG_MAX
#define LONG_MAX	((long)(ULONG_MAX >> 1))	// 0x7FFFFFFF
#endif

#ifndef LONG_MIN
#define LONG_MIN	((long)(~LONG_MAX))			// 0x80000000
#endif

/*
 * Convert a string to a long integer.
 */
long strtol(const char *nptr, char **endptr, int base) {
	const char *s = nptr;
	int c;
	unsigned long cutoff, acc;
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

	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;

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
		acc = neg ? LONG_MIN : LONG_MAX;
	else if (neg)
		acc = -acc;

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return acc;
}
