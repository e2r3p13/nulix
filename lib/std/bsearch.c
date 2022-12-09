// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/bsearch.c
 *
 * Search in a array.
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <stddef.h>

void *bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
	const void *ptr;

	for (size_t i = 0; i < nitems; i++) {
		ptr = base + (i * size);
		if (compar(key, ptr) == 0) {
			return (void *)ptr;
		}
	}
	return NULL;
}
