// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc.c
 *
 * Kmalloc functions
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#include <stddef.h>
#include <kernel/kmalloc.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>
#include <kernel/bitmaptree.h>
#include <kernel/kpm.h>

#include "kmalloc_internal.h"

#include <kernel/print.h>

extern uint32_t ek;
virtaddr_t brkptr = VIRTADDR(&ek);

static struct kmalloc km;
static struct km_block km_initial_block;

static struct bitmap km_bmtlayers[KM_BMT_NLAYERS];
static uint8_t km_bmtarray[KM_BMT_SIZE_ARRAY];

__attribute__ ((section(".heap")))
static uint8_t km_initial_memory[KM_INITIAL_SIZE];

int kmalloc_init() {
	km_initial_block.vaddr = km_initial_memory;
	km_initial_block.paddr = km_initial_memory - KERNEL_VIRT_OFFSET;
	km_initial_block.size = KM_INITIAL_SIZE;
	km_initial_block.nchunk = km_initial_block.size / KM_MEMORY_CHUNK_SIZE;
	km_initial_block.nallocated = 0;
	if (bitmaptree_init(&km_initial_block.bmt, km_initial_block.nchunk, km_bmtlayers,
				KM_BMT_NLAYERS, km_bmtarray, KM_BMT_SIZE_ARRAY) < 0)
		return -1;

	kspin_init(&km.lock);
	km.nblock = 0;
	TAILQ_INIT(&km.block_list);
	return 0;
}

void *kmalloc(size_t size, int flag) {
	size = ALIGNNEXT(size + sizeof(struct km_header), KM_MEMORY_CHUNK_SIZE);
	size_t requested_chunks = size / KM_MEMORY_CHUNK_SIZE;
	if (kspin_trylock(&km.lock) < 0)
		return VIRTADDR(0);
	struct km_block *block;
	TAILQ_FOREACH(block, &km.block_list, next) {
		if (requested_chunks > block->nchunk - block->nallocated)
			continue;
		int index = bitmaptree_get_fit(&block->bmt, requested_chunks);
		if (index < 0)
			continue;
		int nset = bitmaptree_set_from(&block->bmt, index, requested_chunks, 1);
		if (nset <= 0)
			continue;
		virtaddr_t ptr = VIRTADDR(block->vaddr + (index * KM_MEMORY_CHUNK_SIZE));
		struct km_header *header = (struct km_header *)ptr;
		header->allocated_size = size;
		block->nallocated += nset;
		kspin_drop(&km.lock);
		return VIRTADDR(header->data);
	}
	if (flag == KMF_NOFAIL) {
		block = &km_initial_block;
		if (requested_chunks > block->nchunk - block->nallocated) {
			// FIXME: replace by kernel_panic.
			kspin_drop(&km.lock);
			return VIRTADDR(0);
		}
		int index = bitmaptree_get_fit(&block->bmt, requested_chunks);
		if (index < 0) {
			// FIXME: replace by kernel_panic.
			kspin_drop(&km.lock);
			return VIRTADDR(0);
		}
		int nset = bitmaptree_set_from(&block->bmt, index, requested_chunks, 1);
		if (nset <= 0 || (size_t)nset != requested_chunks) {
			// FIXME: replace by kernel_panic.
			kspin_drop(&km.lock);
			return VIRTADDR(0);
		}
		virtaddr_t ptr = VIRTADDR(block->vaddr + (index * KM_MEMORY_CHUNK_SIZE));
		struct km_header *header = (struct km_header *)ptr;
		header->allocated_size = size;
		block->nallocated += requested_chunks;
		kspin_drop(&km.lock);
		return VIRTADDR(header->data);
	} else if (flag == KMF_NORMAL && 0) { // TODO: Remove && 0
		kspin_drop(&km.lock);
		size_t zonesize = ALIGNNEXT(size, (KM_MEMORY_CHUNK_SIZE << (KM_BMT_NLAYERS - 1)));
		physaddr_t zone = kpm_alloc_zone(zonesize);
		if (!zone)
			return VIRTADDR(0);
		struct km_block *new = (struct km_block *)kmalloc(sizeof(struct km_block), KMF_NOFAIL);
		if (!new)
			return VIRTADDR(0);
		new->paddr = PHYSADDR(zone);
		new->size = zonesize;
		new->nchunk = zonesize / KM_MEMORY_CHUNK_SIZE;
		new->nallocated = 0;
		if (bitmaptree_alloc(&new->bmt, new->nchunk, KM_BMT_NLAYERS, KMF_NOFAIL) < 0) {
			kfree(new);
			//TODO: Add kpm_free
			return VIRTADDR(0);
		}
		TAILQ_INSERT_TAIL(&km.block_list, new, next);
		// TODO: Use mapping function to map the new zone to virtual space and alloc the needed space.
		kspin_drop(&km.lock);
		return VIRTADDR(0);
	}
	kspin_drop(&km.lock);
	return VIRTADDR(0);
}

// Check emergency pool after all block
void kfree(void *addr) {
//	struct kmalloc_node *node;
//	TAILQ_FOREACH(node, &km.nodes_list, next) {
//		if (addr == node->address) {
//			size_t index = (node->address - km.start) / KMALLOC_MEMORY_ALIGN;
//			if (bitmaptree_set_from(&km.kbmt, index, node->size, 0) < 0)
//				break;
//			TAILQ_REMOVE(&km.nodes_list, node, next);
//			slab_free(&km.node_slab, node);
//		} else if (addr > node->address && addr < node->address + node->size) {
//			break;
//		}
//	}
}

void *krealloc(void *addr, size_t size, int flag) {
	return VIRTADDR(0);
}

void *kzalloc(size_t size, int flag) {
	void *ptr = kmalloc(size, flag); 
	if (!ptr)
		return NULL;
	memset(ptr, 0, ALIGNNEXT(size, KM_MEMORY_CHUNK_SIZE));
	return ptr;
}
