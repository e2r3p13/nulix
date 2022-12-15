// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/boot_init.c
 *
 * Initialize temporary boot pages and load them in cr3
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/paging.h>
#include <kernel/kernel.h>

/*
 * Cleat a page entry by setting all bits to 0.
 */
#define CLEAR(page, size) \
for (int i = 0; i < PAGE_SIZE; i++) {\
	uint32_t *ptr = (uint32_t *)(page + i); \
	*ptr = 0; \
}

/*
 * Initialize the page entry in present and writable and set physical address.
 */
#define INIT(page_entry, addr) do {\
	(page_entry).present = 1; \
	(page_entry).writable = 1; \
	(page_entry).address = (addr) >> 12; \
} while (0)

/*
 * Create a page directory and a page table and map the first 4M at virtual
 * address 0x0 and 0xc0000000.
 */
void boot_init() {
	struct page_entry *page_directory = (struct page_entry *)0x1000;
	struct page_entry *page_table1 = (struct page_entry *)0x2000;

	CLEAR(page_directory, PAGE_DIRECTORY_LENGTH);
	INIT(page_directory[LAST_PAGE_ENTRY], (uint32_t)page_directory);

	CLEAR(page_table1, PAGE_TABLE_LENGTH);

	for (uint32_t pindex = 0; pindex < PAGE_TABLE_LENGTH; pindex++)
		INIT(page_table1[pindex], pindex * PAGE_SIZE);

	INIT(page_directory[0], (uint32_t)page_table1);
	INIT(page_directory[KERNEL_VIRT_OFFSET >> 22], (uint32_t)page_table1);

	// Load the page directory to cr3 to tell the cpu to using this page directory
	// to resolve virtual address.
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uint32_t)page_directory));
}
