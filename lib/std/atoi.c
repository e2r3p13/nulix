// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/atoi.c
 *
 * Translate string to number.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <stddef.h>

#include <kernel/stdlib.h>

int atoi(const char *nptr) {
	return (int) strtol(nptr, (char **)NULL, 10); 
}
