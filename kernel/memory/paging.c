// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/paging.c
 *
 * Paging related functions
 * 
 * created: 2022/12/16 - glafond- <glafond-@student.42.fr>
 * updated: 2022/12/16 - glafond- <glafond-@student.42.fr>
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
