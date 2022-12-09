// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/strtoull.c
 *
 * Translate string to number.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/stdlib.h>

/*
 * Convert a string to a unsigned long long integer.
 */
unsigned long long strtoull(const char *nptr, char **endptr, int base) {
	return (unsigned long long)strtoll(nptr, endptr, base);
}
