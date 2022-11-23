// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kpm.c
 *
 * Kernel Physical Memory management
 *
 * created: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/02 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/print.h>
#include <kernel/vga.h>
#include <kernel/string.h>

buddy_t *buddy;

extern uint32_t sk;
extern uint32_t ek;

void buddy_print() {
	VGA_setforegroundcolor(VGA_COLOR_MAGENTA);
	kprintf("-- Buddy allocator --\n");
	VGA_setforegroundcolor(VGA_COLOR_WHITE);

	kprintf("buddy address:        %p\n", buddy);
	kprintf("buddy size:           %u KB\n", buddy->size * 1024);
	kprintf("orders address:       %p\n", buddy->orders);
	kprintf("block number:         %x\n", buddy->nframes);
	kprintf("total blocks size:    %u KB\n\n", (buddy->nframes << 2));
}

static inline size_t compute_orders_size() {
	size_t nbytes = 0;

	for (size_t i = 0, nblocks = buddy->nframes; i < KPM_NORDERS; i++, nblocks /= 2) {
		nbytes += KPM_NBYTES_FROM_NBITS(nblocks);
	}
	return nbytes;
}

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
	size_t orders_size;

	buddy = (buddy_t *)ALIGNNEXT((uint32_t)&ek, PAGE_SIZE);
	buddy->nframes = ALIGN(memkb * 1024 / PAGE_SIZE, 1024);
	enabled_frames_size = KPM_NBYTES_FROM_NBITS(buddy->nframes / 8);
	orders_size = compute_orders_size();
	buddy->size = sizeof(buddy_t) + enabled_frames_size + orders_size;
	buddy->enabled_frames = (void *)buddy + sizeof(buddy_t);

	buddy->orders[0] = (void *)buddy->enabled_frames + enabled_frames_size;
	for (size_t i = 1, prev_nblocks = buddy->nframes; i < KPM_NORDERS; i++, prev_nblocks /= 2) {
		buddy->orders[i] = (void *)buddy->orders[i - 1] + KPM_NBYTES_FROM_NBITS(prev_nblocks);
	}

	memset(buddy->enabled_frames, 0, enabled_frames_size);
	memset(buddy->orders[0], 0xff, orders_size);

	for (struct multiboot_mmap_entry *entry = entries; entry->size != 0; entry++) {
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
			kpm_enable((void *)(uintptr_t)entry->addr, (uint32_t)(entry->len));
	}
	kpm_disable((void *)0, PAGE_SIZE); // Also disables IDT + GDT by design
	kpm_disable(&sk, (void *)&ek - (void *)&sk);
	kpm_disable(buddy, buddy->size);
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

		if (KPM_IS_ALLOCATED(n - 1, lchild_index) || KPM_IS_ALLOCATED(n - 1, rchild_index))
			KPM_ALLOC(n, index);
		else
			KPM_FREE(n, index);

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
	for (int i = 0; i < limit / PAGE_SIZE; i++) {
		KPM_ENABLE(base_index + i);
		KPM_FREE(0, base_index + i);
	}

	for (size_t n = 1; n < KPM_NORDERS; n++)
		kpm_update_order(n, base, limit);
}

/*
 * Set pageframes as unvailable
 *
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
	for (int i = 0; i < limit / PAGE_SIZE; i++) {
		KPM_DISABLE(base_index + i);
		KPM_ALLOC(0, base_index + i);
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
	return KPM_IS_ENABLED((uintptr_t)addr / PAGE_SIZE);
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
	return KPM_IS_ALLOCATED(0, (uintptr_t)addr / PAGE_SIZE);
}

void *kpm_alloc(size_t size) {

}

void kpm_free(void *addr) {

}
