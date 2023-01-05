// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kpm.c
 *
 * Kernel Physical Memory management
 *
 * created: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/05 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/kernel.h>
#include <kernel/bitmap.h>

#include <kernel/list.h>

buddy_t *buddy;

extern uint32_t sk;
extern uint32_t ek;

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
void kpm_init(struct multiboot_mmap_entry *entries, size_t count, size_t memkb) {
	size_t buddy_size;
	size_t enabled_frames_size;
	size_t total_orders_size;

	buddy = (buddy_t *)ALIGNNEXT((uint32_t)&ek, PAGE_SIZE);
	buddy->nframes = ALIGN(memkb * 1024 / PAGE_SIZE, 1024);
	buddy->nfree = 0;
	enabled_frames_size = KPM_NBYTES_FROM_NBITS(buddy->nframes);

	bitmap_init(&buddy->enabled_frames,
			buddy->nframes,
			(uint8_t *)buddy + sizeof(buddy_t),
			enabled_frames_size);

	size_t nblocks = buddy->nframes;
	bitmap_init(&buddy->orders[0],
			nblocks,
			(uint8_t *)buddy->enabled_frames.array + enabled_frames_size,
			KPM_NBYTES_FROM_NBITS(nblocks));

	total_orders_size = KPM_NBYTES_FROM_NBITS(nblocks);
	nblocks /= 2;
	for (size_t i = 1; i < KPM_NORDERS; i++, nblocks /= 2) {
		size_t order_size = KPM_NBYTES_FROM_NBITS(nblocks);
		bitmap_init(&buddy->orders[i],
				nblocks,
				buddy->orders[i - 1].array + KPM_NBYTES_FROM_NBITS(nblocks * 2),
				order_size);
		total_orders_size += order_size;
	}

	buddy->size = sizeof(buddy_t) + enabled_frames_size + total_orders_size;

	for (size_t i = 0; i < KPM_NORDERS; i++)
		bitmap_set_all(&buddy->orders[i], 1);

	for (struct multiboot_mmap_entry *entry = entries; entry->size != 0; entry++) {
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
			kpm_enable((void *)(uintptr_t)entry->addr, (uint32_t)(entry->len));
	}
	kpm_disable((void *)0, PAGE_SIZE); // Also disables IDT + GDT by design
	kpm_disable(&sk, ((uintptr_t)&ek - KERNEL_VIRT_OFFSET) - (uintptr_t)&sk);
	kpm_disable((void *)((uintptr_t)buddy - KERNEL_VIRT_OFFSET), buddy->size);
	kpm_disable((void *)0x400000, PAGE_SIZE * 1024);
}

/*
 * Updates the buddy allocator on a memory range
 *
 * After each operation on the physical memory (alloc/enable...), we need to
 * update every parent nodes in the tree.
 * kpm_update_order does it for a specific order at index @n.
 */
inline static void kpm_update_order(size_t n, void *base, size_t limit) {
	size_t order_block_size = PAGE_SIZE * (1 << n);
	size_t index, lchild_index, rchild_index;
	void *end = base + limit;

	while (base < end) {
		index = ((uintptr_t)base >> n) / PAGE_SIZE; // (base / 2^n) / PAGE_SIZE
		lchild_index = index * 2;
		rchild_index = lchild_index + 1;

		int val = bitmap_get_at(&buddy->orders[n - 1], lchild_index)
				+ bitmap_get_at(&buddy->orders[n - 1], rchild_index);
		bitmap_set_at(&buddy->orders[n], index, val);

		base = (void *)ALIGNNEXTFORCE(base, order_block_size);
	}
}

/*
 * Set pageframes as available
 *
 * @base: Start address of the memory region to enable
 * @limit: Size of the region
 *
 */
void kpm_enable(void *base, size_t limit) {
	if (!ISALIGNED(base, PAGE_SIZE))
		return;
	limit = ALIGN(limit, PAGE_SIZE);

	size_t base_index = (uintptr_t)base / PAGE_SIZE;
	if ((uintptr_t)(base + limit) / PAGE_SIZE > buddy->nframes)
		limit = (buddy->nframes * PAGE_SIZE) - (uintptr_t)base;

	for (int i = 0; i < limit / PAGE_SIZE; i++) {
		if (!bitmap_get_at(&buddy->enabled_frames, base_index + i)) {
			bitmap_set_at(&buddy->enabled_frames, base_index + i, 1);
			bitmap_set_at(&buddy->orders[0], base_index + i, 0);
			buddy->nfree++;
		}
	}

	for (size_t n = 1; n < KPM_NORDERS; n++)
		kpm_update_order(n, base, limit);
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
void kpm_disable(void *base, size_t limit) {
	if (!ISALIGNED(base, PAGE_SIZE))
		return;
	if (!ISALIGNED(limit, PAGE_SIZE))
		limit = ALIGNNEXT(limit, PAGE_SIZE);

	size_t base_index = (uintptr_t)base / PAGE_SIZE;
	if ((uintptr_t)(base + limit) / PAGE_SIZE > buddy->nframes)
		limit = (buddy->nframes * PAGE_SIZE) - (uintptr_t)base;

	for (int i = 0; i < limit / PAGE_SIZE; i++) {
		if (bitmap_get_at(&buddy->enabled_frames, base_index + i)) {
			bitmap_set_at(&buddy->enabled_frames, base_index + i, 0);
			bitmap_set_at(&buddy->orders[0], base_index + i, 1);
			if (buddy->nfree)
				buddy->nfree--;
		}
	}

	for (size_t n = 1; n < KPM_NORDERS; n++)
		kpm_update_order(n, base, limit);
}

/*
 * Return 1 if the address is on a available page frame.
 * Iterate while the page frame containing the address is equal to KPM_PARTENABLED,
 * when it's done two possibility arise, the page frame is available or not.
 *
 * @addr: the address to check
 *
 */
int kpm_isenabled(void *addr) {
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
int kpm_isalloc(void *addr) {
	return bitmap_get_at(&buddy->orders[0], (uintptr_t)addr / PAGE_SIZE);
}

static int find_best_fit_order(size_t size) {
	size_t nframes;

	nframes = ALIGNNEXT(size, PAGE_SIZE) / PAGE_SIZE;
	for (int n = 0; n < KPM_NORDERS; n++) {
		size_t nframes_per_block = 1 << n;
		if (nframes_per_block >= nframes)
			return n;
	}
	return KPM_NORDERS - 1;
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
int kpm_alloc_chunk(kpm_chunk_t *chunk, size_t size) {
	size_t best_fit_order;
	size_t base_index;
	size_t frames_per_block;

	best_fit_order = find_best_fit_order(size);
	for (int o = best_fit_order; o >= 0; o--) {
		int i = bitmap_get_first_zero(&buddy->orders[o]);
		if (i >= 0) {
			frames_per_block = 1 << o;
			base_index = i * frames_per_block;
			for (size_t j = 0; j < frames_per_block; j++) {
				bitmap_set_at(&buddy->orders[0], base_index + j, 1);
				if (buddy->nfree)
					buddy->nfree--;
			}
			chunk->addr = (void *)(i * PAGE_SIZE * frames_per_block);
			chunk->size = PAGE_SIZE * frames_per_block;
			for (size_t n = 1; n < KPM_NORDERS; n++)
				kpm_update_order(n, chunk->addr, chunk->size);
			return 0;
		}
	}
	return -1;
}

static uint8_t slab[(1024 * PAGE_SIZE) / sizeof(struct kpm_chunk)] = {0};

void *kpm_chunk_slab_alloc() {
	for (int i = 0; i < 1024; i++) {
		if (slab[i] == 0) {
			slab[i] = 1;
			return (void *)(0xf0000000 + (i * sizeof(struct kpm_chunk))); 
		}
	}
	return NULL;
}

void kpm_chunk_slab_free(void *ptr) {
	slab[((uintptr_t)ptr - 0xf0000000) / sizeof(struct kpm_chunk)] = 0;
}

int kpm_alloc(struct kpm_chunk_head *head, size_t size) {

	if (ALIGNNEXT(size, PAGE_SIZE) / PAGE_SIZE > buddy->nfree)
		return -1;

	TAILQ_INIT(head);

	while (size) {
		struct kpm_chunk *c = (struct kpm_chunk *)kpm_chunk_slab_alloc();
		if (!c) {
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, list);
				kpm_chunk_slab_free(c);
			}
			return -1;
		}
		if (kpm_alloc_chunk(c, size) < 0) {
			kpm_chunk_slab_free(c);
			while ((c = TAILQ_FIRST(head))) {
				TAILQ_REMOVE(head, c, list);
				kpm_chunk_slab_free(c);
			}
			return -1;
		}
		TAILQ_INSERT_TAIL(head, c, list);
		if (size < c->size)
			break;
		size -= c->size;
	}
	return 0;
}

void kpm_free_chunk(kpm_chunk_t *chunk) {
	size_t nframes;
	size_t first_frame_index;

	if (!ISALIGNED(chunk->addr, PAGE_SIZE)) 
		return;

	first_frame_index = (uintptr_t)chunk->addr / PAGE_SIZE;
	nframes = ALIGNNEXT(chunk->size, PAGE_SIZE) / PAGE_SIZE;
	
	for (size_t i = first_frame_index; i < first_frame_index + nframes; i++) {
		if (bitmap_get_at(&buddy->enabled_frames, i))
			bitmap_set_at(&buddy->orders[0], i, 0);
	}
	for (size_t n = 1; n < KPM_NORDERS; n++)
		kpm_update_order(n, chunk->addr, chunk->size);
}

void kpm_free(struct kpm_chunk_head *head) {
	struct kpm_chunk *c;
	while ((c = TAILQ_FIRST(head))) {
		TAILQ_REMOVE(head, c, list);
		kpm_free_chunk(c);
		kpm_chunk_slab_free(c);
	}
}
