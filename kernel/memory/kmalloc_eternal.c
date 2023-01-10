// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc_eternal.c
 *
 * Kmalloc eternal functions
 *
 * created: 2023/01/10 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/kmalloc_eternal.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>

static struct kmalloc_eternal keternal;

int kmalloc_eternal_init() {
	keternal.pchunk.addr = (void *)PHYSMEM_KMETERNAL_ADDR;
	keternal.pchunk.size = PHYSMEM_KMETERNAL_SIZE; 
	keternal.start = (void *)(PHYSMEM_KMETERNAL_ADDR + KERNEL_VIRT_OFFSET);
	keternal.end = keternal.start + PHYSMEM_KMETERNAL_SIZE;
	keternal.cur = keternal.start;
	return 0;
}

void *kmalloc_eternal(size_t size) {
	virtaddr_t oldcur = keternal.cur;
	virtaddr_t newcur = oldcur + ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN);
	if (newcur > keternal.end)
		return NULL;
	keternal.cur = newcur;
	return oldcur;
}

void *kzalloc_eternal(size_t size) {
	void *ptr = kmalloc_eternal(size); 
	if (!ptr)
		return NULL;
	memset(ptr, 0, ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN));
	return ptr;
}
