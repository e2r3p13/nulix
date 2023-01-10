// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kpm.c
 *
 * Kernel Physical Memory management
 *
 * created: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/kernel.h>
#include <kernel/bitmap.h>
#include <kernel/bitmaptree.h>
#include <kernel/slab.h>
#include <kernel/memory.h>

#include <kernel/list.h>

#define KPM_MAX_BUDDY_SIZE ((((uint32_t)(-1) / PAGE_SIZE) / 8) * 2)

buddy_t *buddy;
struct bitmap orders[KPM_NORDERS];
struct slab chunk_slab;

extern uint32_t sk;
extern uint32_t ek;

__attribute__ ((section(".buddy"))) static uint8_t kpm_buddy_reserved_memory[KPM_MAX_BUDDY_SIZE];

/*
 * kpm_init must be called before any call to other kpm functions.
 * It initializes the buddy structure in memory and set the available
 * RAM regions as free blocks.
 *
 * @entries: Address of the multiboot structure array containing information on
 * memory maps from the BIOS
 * @count: Number of memory map entries
 * @memkb: Total amount of physical memory, in KiB
 */
int kpm_init(struct multiboot_mmap_entry *entries, size_t count, size_t memkb) {

	buddy = (buddy_t *)kpm_buddy_reserved_memory;
	buddy->nframes = ALIGN(memkb * 1024 / PAGE_SIZE, 1024);
	buddy->nfree = 0;

	size_t enabled_frames_size = BM_NBYTES_FROM_NBITS(buddy->nframes);
	if (bitmap_init(&buddy->enabled_frames,
			buddy->nframes,
			(uint8_t *)buddy + sizeof(buddy_t),
			enabled_frames_size))
		return -1;

	size_t total_orders_size = 0;
	size_t nblocks = buddy->nframes;
	for (size_t i = 0; i < KPM_NORDERS; i++, nblocks /= 2)
		total_orders_size += BM_NBYTES_FROM_NBITS(nblocks);

	if (bitmaptree_init(&buddy->orders,
			buddy->nframes,
			orders,
			KPM_NORDERS,
			(uint8_t *)buddy->enabled_frames.array + enabled_frames_size,
			total_orders_size))
		return -1;
	if (bitmaptree_set_from(&buddy->orders, 0, buddy->orders.nleafs, 1) < 0)
		return -1;

	buddy->size = sizeof(buddy_t) + enabled_frames_size + total_orders_size;

	for (struct multiboot_mmap_entry *entry = entries; entry->size != 0; entry++) {
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
			if (kpm_enable((void *)(uintptr_t)entry->addr, (uint32_t)(entry->len)))
				return -1;
		}
	}

	if (slab_init(&chunk_slab, buddy->nframes / 2, sizeof(struct kpm_chunk)) < 0)
		return -1;

	if (kpm_disable((void *)0, 1 * MB) < 0)
		return -1;
	if (kpm_disable(&sk, ((uintptr_t)&ek - KERNEL_VIRT_OFFSET)) < 0)
		return -1;

	return 0;
}

/*
 * Set pageframes as available
 *
 * @base: Start address of the memory region to enable
 * @limit: Size of the region
 *
 */
int kpm_enable(physaddr_t base, size_t limit) {
	if (!ISALIGNED(base, PAGE_SIZE))
		return -1;
	limit = ALIGN(limit, PAGE_SIZE);

	size_t base_index = (uintptr_t)base / PAGE_SIZE;
	if ((uintptr_t)(base + limit) / PAGE_SIZE > buddy->nframes)
		limit = (buddy->nframes * PAGE_SIZE) - (uintptr_t)base;

	size_t len = limit / PAGE_SIZE;
	bitmap_set_from(&buddy->enabled_frames, base_index, len, 1);
	size_t nset = bitmaptree_set_from(&buddy->orders, base_index, len, 0);
	if (nset < 0)
		return -1;
	buddy->nfree += nset;
	return 0;
}

/*
 * Set pageframes as unvailable
 *
		buddy->nfree++;
		buddy->nfree++;
 * @base: Start address of the memory region to enable
 * @limit: Size of the region
 *
 */
int kpm_disable(physaddr_t base, size_t limit) {
	if (!ISALIGNED(base, PAGE_SIZE))
		return -1;
	if (!ISALIGNED(limit, PAGE_SIZE))
		limit = ALIGNNEXT(limit, PAGE_SIZE);

	size_t base_index = (uintptr_t)base / PAGE_SIZE;
	if ((uintptr_t)(base + limit) / PAGE_SIZE > buddy->nframes)
		limit = (buddy->nframes * PAGE_SIZE) - (uintptr_t)base;

	size_t len = limit / PAGE_SIZE;
	bitmap_set_from(&buddy->enabled_frames, base_index, len, 0);
	size_t nset = bitmaptree_set_from(&buddy->orders, base_index, len, 1);
	if (nset < 0)
		return -1;
	buddy->nfree -= nset;
	return 0;
}

/*
 * Return 1 if the address is on a available page frame.
 * Iterate while the page frame containing the address is equal to KPM_PARTENABLED,
 * when it's done two possibility arise, the page frame is available or not.
 *
 * @addr: the address to check
 *
 */
int kpm_isenabled(physaddr_t addr) {
	return bitmap_get_at(&buddy->enabled_frames, (uintptr_t)addr / PAGE_SIZE);
}

/*
 * Return 1 if the address is on a allocated page frame.
 * Iterate while the page frame containing the address is equal to KPM_PARTENABLED
 * than if is equal to KPM_PARTALLOCATED, when it's done two possibility arise,
 * the page frame is allocated or not.
 *
 * @addr: the address to check
 *
 */
int kpm_isalloc(physaddr_t addr) {
	return bitmap_get_at(&buddy->orders.layers[0], (uintptr_t)addr / PAGE_SIZE);
}

/*
 * kpm_alloc searches for the biggest contiguous region, up to
 * @size bytes, and fills the struct @chunk with this candidate.
 *
 * Returns 0 on success, -1 on error
 *
 * NOTE: the returned chunk may be smaller than the requested size if there
 * is no contiguous block big enough. In this case, subsequent calls will
 * be needed to get the remaining chunks.
 */
physaddr_t kpm_alloc_zone(size_t size) {
	size_t len = size / PAGE_SIZE;
	int index = bitmaptree_get_fit(&buddy->orders, len);
	if (index <= 0)
		return PHYSADDR(0);
	size_t nset = bitmaptree_set_from(&buddy->orders, (size_t)index, len, 1);
	if (nset < 0)
		return PHYSADDR(0);
	buddy->nfree -= nset;
	return PHYSADDR(index * PAGE_SIZE);
}


int kpm_alloc_chunk(kpm_chunk_t *chunk, size_t size) {
	size_t len = size / PAGE_SIZE;
	for (;len > 0; len /= 2) {
		int index = bitmaptree_get_fit(&buddy->orders, len);
		if (index < 0)
			continue;
		size_t nset = bitmaptree_set_from(&buddy->orders, (size_t)index, len, 1);
		if (nset < 0)
			return -1;
		buddy->nfree -= nset;
		chunk->addr = (void *)(index * PAGE_SIZE);
		chunk->size = len * PAGE_SIZE;
		return 0;
	}
	return -1;
}

int kpm_alloc(struct kpm_chunk_head *head, size_t size) {

	if (ALIGNNEXT(size, PAGE_SIZE) / PAGE_SIZE > buddy->nfree) {
		return -1;
	}

	TAILQ_INIT(head);

	while (size) {
		struct kpm_chunk *c = (struct kpm_chunk *)slab_alloc(&chunk_slab);
		if (!c) {
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, list);
				slab_free(&chunk_slab, c);
			}
			return -1;
		}
		if (kpm_alloc_chunk(c, size) < 0) {
			slab_free(&chunk_slab, c);
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, list);
				slab_free(&chunk_slab, c);
			}
			kprintf("2\n");
			return -1;
		}
		TAILQ_INSERT_TAIL(head, c, list);
		if (size < c->size)
			break;
		size -= c->size;
	}
	return 0;
}

int kpm_free_chunk(kpm_chunk_t *chunk) {
	if (!ISALIGNED(chunk->addr, PAGE_SIZE)) 
		return -1;
	if (!ISALIGNED(chunk->size, PAGE_SIZE)) 
		return -1;

	if (bitmaptree_set_from_if(&buddy->orders,
				&buddy->enabled_frames,
				(size_t)chunk->addr / PAGE_SIZE,
				chunk->size / PAGE_SIZE,
				0) < 0)
		return -1;
	return 0;
}

int kpm_free(struct kpm_chunk_head *head) {
	struct kpm_chunk *c;
	while ((c = TAILQ_FIRST(head))) {
		if (kpm_free_chunk(c))
			return -1;
		TAILQ_REMOVE(head, c, list);
		slab_free(&chunk_slab, c);
	}
}
