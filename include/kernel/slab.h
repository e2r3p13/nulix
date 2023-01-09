// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/slab.h
 *
 * 
 * Slab allocator header
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
 */

#ifndef SLAB_H
#define SLAB_H

#include <kernel/bitmap.h>

struct slab {
	size_t objsize;
	size_t nobj;
	void *zone;
	struct bitmap bm;
};

int slab_init(struct slab *slab, size_t nobj, size_t objsize);

void *slab_alloc(struct slab *slab);
int slab_free(struct slab *slab, void *ptr);

#endif
