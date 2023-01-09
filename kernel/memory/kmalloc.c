// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc.c
 *
 * Kmalloc functions
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/kmalloc.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>

static struct kmalloc km;

int kmalloc_init_eternal() {
	km.eternal.pchunk.addr = (void *)PHYSMEM_KMETERNAL_ADDR;
	km.eternal.pchunk.size = PHYSMEM_KMETERNAL_SIZE; 
	km.eternal.start = km.eternal.pchunk.addr + KERNEL_VIRT_OFFSET;
	km.eternal.end = km.eternal.start + km.eternal.pchunk.size;
	km.eternal.cur = km.eternal.start;
}

static void *kmalloc_eternal(size_t size) {
	virtaddr_t oldcur = km.eternal.cur;
	virtaddr_t newcur = oldcur + ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN);
	if (newcur > km.eternal.end)
		return NULL;
	km.eternal.cur = newcur;
	return oldcur;
}

void *kmalloc(size_t size, int flags) {
	switch (flags) {
		case KMZ_ETERNAL:
			return kmalloc_eternal(size);
		default:
			return NULL;
	}
}

void *kzalloc(size_t size, int flags) {
	void *ptr = kmalloc(size, flags); 
	if (!ptr)
		return NULL;
	memset(ptr, 0, ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN));
	return ptr;
}

void kfree(void *addr) {
	return;
}
