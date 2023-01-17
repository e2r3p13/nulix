// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc.c
 *
 * Kmalloc functions
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <stddef.h>
#include <kernel/kmalloc.h>
#include <kernel/memory.h>
#include <kernel/kernel.h>
#include <kernel/string.h>
#include <kernel/bitmaptree.h>
#include <kernel/kpm.h>
#include <kernel/symbole.h>
#include <kernel/vmm.h>

#include "kmalloc_internal.h"

#include <kernel/print.h>

virtaddr_t brkptr = VIRTADDR(NULL);

static struct kmalloc km;
static struct km_block km_initial_block;

static struct bitmap km_bmtlayers[KM_BMT_NLAYERS];
static uint8_t km_bmtarray[KM_BMT_SIZE_ARRAY];

__attribute__ ((section(".heap")))
static uint8_t km_initial_memory[KM_INITIAL_SIZE];

/*
 * Initialize the first memory block and the kmalloc struct.
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
	brkptr = VIRTADDR(SYM_VIRT_HEAP + SYM_HEAP_SIZE);
	return 0;
}

/*
 * Return the index where @chunks can fit in @block.
 */
static int km_check_if_fit_block(struct km_block *block, size_t chunks) {
	int index = -1;
	if (chunks <= block->nchunk - block->nallocated)
		index = bitmaptree_get_fit(&block->bmt, chunks);
	return index;
}

/*
 * Try to return a pointer of @chunks at @index in the @block bitmap.
 * Otherwise return NULL.
 */
void *km_allocation(struct km_block *block, size_t chunks, int index) {
	if (index >= 0) {
		int nset = bitmaptree_set_from(&block->bmt, index, chunks, 1);
		if (nset <= 0 || nset != chunks)
			return VIRTADDR(NULL);
		virtaddr_t ptr = VIRTADDR(block->vaddr + (index * KM_MEMORY_CHUNK_SIZE));
		struct km_header *header = (struct km_header *)ptr;
		header->allocated_size = nset * KM_MEMORY_CHUNK_SIZE;
		block->nallocated += nset;
		if (header->data)
			return VIRTADDR(header->data);
	}
	return VIRTADDR(NULL);
}

/*
 * Allocate a new km_block of 4MB
 */
struct km_block *km_new_block() {
		size_t zonesize = 4 * MB;
		physaddr_t zone = kpm_alloc_zone(zonesize);
		if (!zone)
			goto km_new_block6;
		struct km_block *new = kmalloc(sizeof(struct km_block), KMF_NOFAIL);
		if (!new)
			goto km_new_block5;
		new->vaddr = brkptr;
		new->paddr = PHYSADDR(zone);
		new->size = zonesize;
		new->nchunk = zonesize / KM_MEMORY_CHUNK_SIZE;
		new->nallocated = 0;
		if (bitmaptree_alloc(&new->bmt, new->nchunk, KM_BMT_NLAYERS, KMF_NOFAIL) < 0)
			goto km_new_block4;
		struct vmobject *obj = vmobject_new_amonymous(zone, zonesize);
		if (!obj)
			goto km_new_block3;
		struct page_entry_flags flags;
		*(uint32_t *)&flags = 0;
		flags.present = 1;
		flags.writable = 1;
		struct vmmap *map;
		struct vmzone *vmzone;

		kspin_lock(&km.lock);
		virtaddr_t tmpbrk = brkptr;
		brkptr += 4 * MB;
		kspin_drop(&km.lock);

		TAILQ_FOREACH(map, &vmmap_list, next) {
			vmzone = vmzone_new(tmpbrkptr, tmpbrkptr + (4 * MB), "kheap", *(uint32_t *)&flags, obj, 0);
			if (!vmzone)
				goto km_new_block2;
			if (vmmap_map_zone(map, vmzone) < 0)
				goto km_new_block1;
		}

		kspin_lock(&km.lock);
		TAILQ_INSERT_TAIL(&km.block_list, new, next);
		kspin_drop(&km.lock);
		return new;
km_new_block1:
		vmzone_delete(vmzone);
km_new_block2:
		vmobject_delete(obj);
km_new_block3:
		bitmaptree_free(&new->bmt);
km_new_block4:
		kfree(new);
km_new_block5:
		kpm_free_zone(zone, zonesize);
km_new_block6:
		return VIRTADDR(NULL);
}

/*
 * Return a pointer to a free virtual memory zone of @size bytes
 *
 * INFO: Big allocation do fragmetation even when there is room in the current
 * block see this as a feature not a bug :)
 */
void *kmalloc(size_t size, int flag) {
	size = ALIGNNEXT(size + sizeof(struct km_header), KM_MEMORY_CHUNK_SIZE);
	if (size > 4 * MB)
		return VIRTADDR(NULL);
	size_t requested_chunks = size / KM_MEMORY_CHUNK_SIZE;

	struct km_block *block = NULL;
	int index = -1;
	kspin_lock(&km.lock);
	TAILQ_FOREACH(block, &km.block_list, next) {
		if (block->nchunk - block->nallocated >= requested_chunks) {
			index = bitmaptree_get_fit(&block->bmt, requested_chunks);
			if (index >= 0)
				break;
		}
	}
kmalloc_end_block_loop:
	if (!block) {
		if (flag == KMF_NOFAIL) {
			block = &km_initial_block;
			index = bitmaptree_get_fit(&block->bmt, requested_chunks);
			virtaddr_t ptr = km_allocation(block, requested_chunks, index);	
			kspin_drop(&km.lock);
			return ptr;
		} else if (flag == KMF_NORMAL) {
			kspin_drop(&km.lock);
			block = km_new_block();
			kspin_lock(&km.lock);
			index = bitmaptree_get_fit(&block->bmt, requested_chunks);
			goto kmalloc_end_block_loop;
		} else {
			kspin_drop(&km.lock);
			return VIRTADDR(NULL);
		}
	} else {
		virtaddr_t ptr = km_allocation(block, requested_chunks, index);	
		kspin_drop(&km.lock);
		return ptr;
	}
}

/*
 * Free a virtual memory zone at @addr previously returned by kmalloc
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
		if (VIRTADDR(kmh->data + kmh->allocated_size) > block->vaddr + block->size)
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
	if (VIRTADDR(kmh->data + kmh->allocated_size) > block->vaddr + block->size) {
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
 * Try to increase/decrease the reserved memory zone or allocate a new one and copy 
 * data in it.
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
				|| VIRTADDR(kmh) > block->vaddr + block->size
				|| VIRTADDR(kmh->data + kmh->allocated_size) > block->vaddr + block->size)
			continue ;
		break;
	}
	if (!block && flag == KMF_NOFAIL) {
		block = &km_initial_block;
		if ((virtaddr_t)kmh < block->vaddr
				|| VIRTADDR(kmh) > block->vaddr + block->size
				|| VIRTADDR(kmh->data + kmh->allocated_size) > block->vaddr + block->size) {
			kspin_drop(&km.lock);
			return VIRTADDR(NULL);
		}
	}
	if (kmh->allocated_size > size) {
		virtaddr_t  ptr = VIRTADDR(VIRTADDR(kmh) - block->vaddr);
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
 * Call kmalloc and set the memory zone to zero.
 */
void *kzalloc(size_t size, int flag) {
	void *ptr = kmalloc(size, flag); 
	if (!ptr)
		return VIRTADDR(NULL);
	memset(ptr, 0, ALIGNNEXT(size, KM_MEMORY_CHUNK_SIZE));
	return VIRTADDR(ptr);
}
