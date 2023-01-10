// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/slab.c
 *
 * Slab allocator
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/slab.h>
#include <kernel/memory.h>
#include <kernel/kmalloc.h>
#include <kernel/bitmap.h>
#include <kernel/string.h>

int slab_init(struct slab *slab, size_t nobj, size_t objsize) {
	if (!nobj || !objsize)
		return -1;

	slab->zone = kzalloc(nobj * objsize, KMF_NOFAIL);
	if (!slab->zone)
		return -1;

	if (bitmap_alloc(&slab->bm, nobj, KMF_NOFAIL))
		return -1;

	slab->objsize = objsize;
	slab->nobj = nobj;

	return 0;
}

void *slab_alloc(struct slab *slab) {
	int index = bitmap_get_first_zero(&slab->bm);
	if (index < 0)
		return NULL;
	if (bitmap_set_at(&slab->bm, index, 1) < 0)
		return NULL;
	return slab->zone + (index * slab->objsize);
}

int slab_free(struct slab *slab, void *ptr) {
	size_t index = (ptr - slab->zone) / slab->objsize;
	if (bitmap_set_at(&slab->bm, index, 0) < 0)
		return -1;
	memset(ptr, 0, slab->objsize);
	return 0;
}
