// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/strtoul.c
 *
 * Translate string to number.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/stdlib.h>

/*
 * Convert a string to a unsigned long integer.
 */
unsigned long strtoul(const char *nptr, char **endptr, int base) {
	return (unsigned long)strtol(nptr, endptr, base);
}
