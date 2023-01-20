// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kpm.c
 *
 * Kernel Physical Memory management
 *
 * created: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/17 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/kernel.h>
#include <kernel/bitmap.h>
#include <kernel/bitmaptree.h>
#include <kernel/memory.h>
#include <kernel/symbole.h>
#include <kernel/kmalloc.h>

#include <kernel/list.h>

// Allocate three times the max size of a bitmap of the number of pages 
// and add 0x200 aproximatly the size of the structs.
#define KPM_MAX_BUDDY_SIZE ((((uint32_t)(-1) / PAGE_SIZE) / 8) * 3) + 0x200

buddy_t *buddy;
struct bitmap orders[KPM_NORDERS];

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

	if (kpm_disable((void *)0, 1 * MB) < 0)
		return -1;
	if (kpm_disable((void *)SYM_KERNEL_START, SYM_KERNEL_END - SYM_KERNEL_START) < 0)
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
 * kpm_alloc_zone searches for contiguous zone of @size bytes and return
 * the physical address of the zone. Otherwise return PHYSADDR(NULL).
 */
physaddr_t kpm_alloc_zone(size_t size) {
	size_t len = ALIGNNEXT(size, PAGE_SIZE) / PAGE_SIZE;
	int index = bitmaptree_get_fit(&buddy->orders, len);
	if (index <= 0)
		return PHYSADDR(NULL);
	size_t nset = bitmaptree_set_from(&buddy->orders, (size_t)index, len, 1);
	if (nset < 0)
		return PHYSADDR(NULL);
	buddy->nfree -= nset;
	return PHYSADDR(index * PAGE_SIZE);
}

/*
 * kpm_alloc_page searches for a free page and return its
 * the physical address of the zone. Otherwise return PHYSADDR(NULL).
 */
physaddr_t kpm_alloc_page() {
	int index = bitmap_get_first_zero(&buddy->orders.layers[0]);
	if (index < 0)
		return PHYSADDR(NULL);
	if (bitmap_set_at(&buddy->orders.layers[0], index, 1) < 0)
		return PHYSADDR(NULL);
	if (bitmaptree_update(&buddy->orders, index, 1) < 0)
		return PHYSADDR(NULL);
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
		struct kpm_chunk *c = (struct kpm_chunk *)kmalloc(sizeof(struct kpm_chunk), KMF_NOFAIL);
		if (!c) {
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, next);
				kfree(c);
			}
			return -1;
		}
		if (kpm_alloc_chunk(c, size) < 0) {
			kfree(c);
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, next);
				kfree(c);
			}
			return -1;
		}
		TAILQ_INSERT_TAIL(head, c, next);
		if (size < c->size)
			break;
		size -= c->size;
	}
	return 0;
}

void kpm_free_page(physaddr_t addr) {
	if (!ISALIGNED(addr, PAGE_SIZE)) 
		return;
	size_t index = (size_t)addr / PAGE_SIZE;
	bitmap_set_at(&buddy->orders.layers[0], index, 0);
	bitmaptree_update(&buddy->orders, index, 1);
}

void kpm_free_zone(physaddr_t addr, size_t size) {
	if (!ISALIGNED(addr, PAGE_SIZE)) 
		return;
	size_t index = (size_t)addr / PAGE_SIZE;
	bitmaptree_set_from(&buddy->orders, index, size / PAGE_SIZE, 0);
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
		TAILQ_REMOVE(head, c, next);
		kfree(c);
	}
}
