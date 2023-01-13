// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/paging.c
 *
 * Paging related functions
 * 
 * created: 2022/12/16 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/13 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/paging.h>

/*
 * Initialize a page entry with given flags
 */
void page_init(struct page_entry *pe, void *addr, int w, int u) {
	*(uintptr_t *)pe = 0;
	pe->present = 1;
	pe->writable = w;
	pe->user = u;
	pe->address = ((uintptr_t)addr >> 12); 
}

/*
 * Clear the page entry
 */
void page_clear(struct page_entry *pe) {
	*(uintptr_t *)pe = 0;
}

/*
 * Return the address loaded in cr3
 */
inline physaddr_t get_cr3() {
	physaddr_t addr;
	__asm__ volatile ("movl %%cr3, %0" :: "r" ((uintptr_t)addr));
	return addr;
}

/*
 * Set an address in cr3
 */
inline void set_cr3(physaddr_t addr) {
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uintptr_t)addr));
}

/*
 * Reload the cr3 to flush the tlb
 */
inline void reload_cr3() {
	set_cr3(get_cr3());
}

