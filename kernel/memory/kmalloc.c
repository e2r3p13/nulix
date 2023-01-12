// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc.c
 *
 * Kmalloc functions
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/12 - glafond- <glafond-@student.42.fr>
 */

#include <stddef.h>
#include <kernel/kmalloc.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>
#include <kernel/bitmaptree.h>
#include <kernel/kpm.h>
#include <kernel/symbole.h>

#include "kmalloc_internal.h"

#include <kernel/print.h>

virtaddr_t brkptr = VIRTADDR(SYM_KERNEL_END);

static struct kmalloc km;
static struct km_block km_initial_block;

static struct bitmap km_bmtlayers[KM_BMT_NLAYERS];
static uint8_t km_bmtarray[KM_BMT_SIZE_ARRAY];

__attribute__ ((section(".heap")))
static uint8_t km_initial_memory[KM_INITIAL_SIZE];

/*
 * ##########################################
 *
 */
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

/*
 * ##########################################
 *
 */
static int km_check_if_fit_block(struct km_block *block, size_t requested_chunks) {
	int index = -1;
	if (requested_chunks <= block->nchunk - block->nallocated)
		index = bitmaptree_get_fit(&block->bmt, requested_chunks);
	return index;
}

static void *km_alloc_from(struct km_block *block, size_t index, size_t requested_chunks) {
	int nset = bitmaptree_set_from(&block->bmt, index, requested_chunks, 1);
	if (nset <= 0 || nset != requested_chunks)
		return VIRTADDR(0);
	virtaddr_t ptr = VIRTADDR(block->vaddr + (index * KM_MEMORY_CHUNK_SIZE));
	struct km_header *header = (struct km_header *)ptr;
	header->allocated_size = nset * KM_MEMORY_CHUNK_SIZE;
	block->nallocated += nset;
	return VIRTADDR(header->data);
}

/*
 * ##########################################
 *
 */
void *kmalloc(size_t size, int flag) {
	size = ALIGNNEXT(size + sizeof(struct km_header), KM_MEMORY_CHUNK_SIZE);
	size_t requested_chunks = size / KM_MEMORY_CHUNK_SIZE;
	kspin_lock(&km.lock);
	struct km_block *block = NULL;
	int index = -1;
	TAILQ_FOREACH(block, &km.block_list, next) {
		index = km_check_if_fit_block(block, requested_chunks);
		if (index >= 0) {
			virtaddr_t ptr = km_alloc_from(block, index, requested_chunks);
			if (!ptr)
				continue;
			kspin_drop(&km.lock);
			return VIRTADDR(ptr);
		}
	}
	if (flag == KMF_NOFAIL) {
		block = &km_initial_block;
		index = km_check_if_fit_block(block, requested_chunks);
		if (index >= 0) {
			virtaddr_t ptr = km_alloc_from(block, index, requested_chunks);
			if (ptr) {
				kspin_drop(&km.lock);
				return VIRTADDR(ptr);
			}
		}
		kspin_drop(&km.lock);
		return VIRTADDR(NULL);
	} else if (flag == KMF_NORMAL && 0) { // TODO: Remove && 0
		kspin_drop(&km.lock);
		size_t zonesize = ALIGNNEXT(size, (KM_MEMORY_CHUNK_SIZE << (KM_BMT_NLAYERS - 1)));
		physaddr_t zone = kpm_alloc_zone(zonesize);
		if (!zone)
			return VIRTADDR(NULL);
		struct km_block *new = (struct km_block *)kmalloc(sizeof(struct km_block), KMF_NOFAIL);
		if (!new)
			return VIRTADDR(NULL);
		new->paddr = PHYSADDR(zone);
		new->size = zonesize;
		new->nchunk = zonesize / KM_MEMORY_CHUNK_SIZE;
		new->nallocated = 0;
		if (bitmaptree_alloc(&new->bmt, new->nchunk, KM_BMT_NLAYERS, KMF_NOFAIL) < 0) {
			kfree(new);
			//TODO: Add kpm_free
			return VIRTADDR(NULL);
		}
		TAILQ_INSERT_TAIL(&km.block_list, new, next);
		// TODO: Use mapping function to map the new zone to virtual space and alloc the needed space.
		kspin_drop(&km.lock);
		return VIRTADDR(NULL);
	}
	kspin_drop(&km.lock);
	return VIRTADDR(NULL);
}

// Check emergency pool after all block
/*
 * ##########################################
 *
 */
void kfree(void *addr) {
	if (!addr)
		return;
	kspin_lock(&km.lock);
	struct km_header *kmh = (struct km_header *)(addr - sizeof(struct km_header));
	struct km_block *block;
	TAILQ_FOREACH(block, &km.block_list, next) {
		if ((virtaddr_t)kmh < block->vaddr)
			break;
		if ((virtaddr_t)kmh > block->vaddr + block->size)
			continue;
		if (kmh->data + kmh->allocated_size > block->vaddr + block->size)
			break;
		size_t index = ((virtaddr_t)kmh - block->vaddr) / KM_MEMORY_CHUNK_SIZE;
		int nset = bitmaptree_set_from(&block->bmt, index, kmh->allocated_size, 0);
		if (nset > 0)
			block->nallocated -= nset;
		kspin_drop(&km.lock);
		return;
	}
	block = &km_initial_block;
	if ((virtaddr_t)kmh < block->vaddr) {
		kspin_drop(&km.lock);
		return;
	}
	if ((virtaddr_t)kmh > block->vaddr + block->size) {
		kspin_drop(&km.lock);
		return;
	}
	if (kmh->data + kmh->allocated_size > block->vaddr + block->size) {
		kspin_drop(&km.lock);
		return;
	}
	size_t index = ((virtaddr_t)kmh - block->vaddr) / KM_MEMORY_CHUNK_SIZE;
	int nset = bitmaptree_set_from(&block->bmt, index, kmh->allocated_size, 0);
	if (nset > 0)
		block->nallocated -= nset;
	kspin_drop(&km.lock);
}

/*
 * ##########################################
 *
 */
void *krealloc(void *addr, size_t size, int flag) {
	if (!addr)
		return VIRTADDR(NULL);
	kspin_lock(&km.lock);
	size = ALIGNNEXT(size + sizeof(struct km_header), KM_MEMORY_CHUNK_SIZE);
	struct km_header *kmh = (struct km_header *)(addr - sizeof(struct km_header));
	struct km_block *block;
	TAILQ_FOREACH(block, &km.block_list, next) {
		if ((virtaddr_t)kmh < block->vaddr
				|| (virtaddr_t)kmh > block->vaddr + block->size
				|| kmh->data + kmh->allocated_size > block->vaddr + block->size)
			continue ;
		break;
	}
	if (!block && flag == KMF_NOFAIL) {
		block = &km_initial_block;
		if ((virtaddr_t)kmh < block->vaddr
				|| (virtaddr_t)kmh > block->vaddr + block->size
				|| kmh->data + kmh->allocated_size > block->vaddr + block->size) {
			kspin_drop(&km.lock);
			return VIRTADDR(NULL);
		}
	}
	if (kmh->allocated_size > size) {
		virtaddr_t  ptr = (virtaddr_t)kmh - block->vaddr;
		size_t index = (size_t)ptr / KM_MEMORY_CHUNK_SIZE;
		size_t ssize = (kmh->allocated_size - size) / KM_MEMORY_CHUNK_SIZE;
		size_t sindex = index + (size / KM_MEMORY_CHUNK_SIZE);
		int nset = bitmaptree_set_from(&block->bmt, sindex, ssize, 0);
		if (nset < 0 || nset != ssize) {
			kspin_drop(&km.lock);
			return VIRTADDR(NULL);
		}
		block->nallocated -= nset;
		kmh->allocated_size = size;
	} else if (kmh->allocated_size < size) {
		size_t index = ((virtaddr_t)kmh - block->vaddr) / KM_MEMORY_CHUNK_SIZE;
		size_t end_index = index + (kmh->allocated_size / KM_MEMORY_CHUNK_SIZE);
		size_t add_size = (size - kmh->allocated_size) / KM_MEMORY_CHUNK_SIZE;
		int i = bitmap_get_next_one(&block->bmt.layers[0], end_index);
		if (i < 0 || i - end_index  >= add_size) {
			int nset = bitmaptree_set_from(&block->bmt, end_index, add_size, 1);
			if (nset <= 0 || nset != add_size) {
				kspin_drop(&km.lock);
				return VIRTADDR(NULL);
			}
			block->nallocated += nset;
			kmh->allocated_size = size;
		} else {
			kspin_drop(&km.lock);
			void *naddr = kmalloc(size - sizeof(struct km_header), flag);
			if (!naddr) {
				kprintf("3\n");
				return VIRTADDR(NULL);
			}
			memcpy(naddr, addr, kmh->allocated_size);
			kfree(addr);
			return VIRTADDR(naddr);
		}
	}
	kspin_drop(&km.lock);
	return VIRTADDR(kmh->data);
}

/*
 * ##########################################
 *
 */
void *kzalloc(size_t size, int flag) {
	void *ptr = kmalloc(size, flag); 
	if (!ptr)
		return VIRTADDR(NULL);
	memset(ptr, 0, ALIGNNEXT(size, KM_MEMORY_CHUNK_SIZE));
	return VIRTADDR(ptr);
}
