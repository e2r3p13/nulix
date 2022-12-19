// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/strtoul.c
 *
 * Translate string to number.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/19 - glafond- <glafond-@student.42.fr>
 */

#ifndef ULONG_MAX
#define ULONG_MAX	((unsigned long)(~0L))		// 0xFFFFFFFF
#endif

/*
 * Convert a string to a unsigned long integer.
 */
unsigned long strtoul(const char *nptr, char **endptr, int base) {
	const char *s = nptr;
	int c;
	unsigned long cutoff, acc;
	int cutlim, any;

	do {
		c = *s++;
	} while (c == ' ');

	if (c == '+') {
		c = *s++;
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	cutoff = ULONG_MAX;
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
		acc = ULONG_MAX;

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return acc;
}
