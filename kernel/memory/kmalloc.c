// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc.c
 *
 * Kmalloc functions
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/kmalloc.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>
#include <kernel/bitmaptree.h>

static struct kmalloc km;

int kmalloc_eternal_init() {
	km.eternal.pchunk.addr = (void *)PHYSMEM_KMETERNAL_ADDR;
	km.eternal.pchunk.size = PHYSMEM_KMETERNAL_SIZE; 
	km.eternal.start = (void *)(PHYSMEM_KMETERNAL_ADDR + KERNEL_VIRT_OFFSET);
	km.eternal.end = km.eternal.start + PHYSMEM_KMETERNAL_SIZE;
	km.eternal.cur = km.eternal.start;
	return 0;
}

static void *kmalloc_eternal(size_t size) {
	virtaddr_t oldcur = km.eternal.cur;
	virtaddr_t newcur = oldcur + ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN);
	if (newcur > km.eternal.end)
		return NULL;
	km.eternal.cur = newcur;
	return oldcur;
}

int kmalloc_fix_init() {
	km.fix.pchunk.addr = (void *)PHYSMEM_KMFIX_ADDR;
	km.fix.pchunk.size = PHYSMEM_KMFIX_SIZE; 
	km.fix.start = (void *)(PHYSMEM_KMFIX_ADDR + KERNEL_VIRT_OFFSET);
	km.fix.end = km.fix.start + PHYSMEM_KMFIX_SIZE;
	size_t nobj = PHYSMEM_KMFIX_SIZE / KMALLOC_MEMORY_ALIGN;
	if (bitmaptree_alloc(&km.fix.kbmt, nobj, KM_BMT_NLAYERS, KMZ_ETERNAL) < 0)
		return -1;
	TAILQ_INIT(&km.fix.nodes_list);
	if (slab_init(&km.fix.node_slab, nobj / 4, sizeof(struct kmalloc_node)) < 0)
		return -1;
	return 0;
}

static void *kmalloc_fix(size_t size) {
	size_t len = ALIGNNEXT(size, KMALLOC_MEMORY_ALIGN) / KMALLOC_MEMORY_ALIGN;
	int index = bitmaptree_get_fit(&km.fix.kbmt, len);
	if (index < 0)
		return NULL;
	struct kmalloc_node *node = slab_alloc(&km.fix.node_slab);
	if (!node)
		return NULL;
	node->address = km.fix.start + (index * KMALLOC_MEMORY_ALIGN);
	node->size = len;
	if (bitmaptree_set_from(&km.fix.kbmt, index, len, 1) < 0) {
		slab_free(&km.fix.node_slab, node);
		return NULL;
	}
	TAILQ_INSERT_TAIL(&km.fix.nodes_list, node, next);
	return node->address;
}

static void kfree_fix(void *addr) {
	struct kmalloc_node *node;
	TAILQ_FOREACH(node, &km.fix.nodes_list, next) {
		if (addr == node->address) {
			size_t index = (node->address - km.fix.start) / KMALLOC_MEMORY_ALIGN;
			if (bitmaptree_set_from(&km.fix.kbmt, index, node->size, 0) < 0)
				break;
			TAILQ_REMOVE(&km.fix.nodes_list, node, next);
			slab_free(&km.fix.node_slab, node);
		} else if (addr > node->address && addr < node->address + node->size) {
			break;
		}
	}
}

void *kmalloc(size_t size, int flags) {
	if (!size)
		return NULL;
	switch (flags) {
		case KMZ_ETERNAL:
			return kmalloc_eternal(size);
		case KMZ_FIX:
			return kmalloc_fix(size);
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
	if (addr >= km.fix.start && addr < km.fix.end)
		kfree_fix(addr);
}
