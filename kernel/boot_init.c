// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/boot_init.c
 *
 * Initialize temporary boot pages and load them in cr3
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/16 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/paging.h>
#include <kernel/kernel.h>
#include <kernel/string.h>

/*
 * Create a page directory and a page table and map the first 4M at virtual
 * address 0x0 and 0xc0000000.
 */
void boot_init() {
	struct page_entry *page_directory = (struct page_entry *)0x1000;
	struct page_entry *page_table1 = (struct page_entry *)0x2000;

	(memset - KERNEL_VIRT_OFFSET)(page_directory, 0, PAGE_DIRECTORY_LENGTH);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory + LAST_PAGE_ENTRY, page_directory, 1, 0);

	(memset - KERNEL_VIRT_OFFSET)(page_directory, 0, PAGE_DIRECTORY_LENGTH);

	for (uint32_t pindex = 0; pindex < PAGE_TABLE_LENGTH; pindex++)
		(page_init - KERNEL_VIRT_OFFSET)(page_table1 + pindex, (void *)(pindex * PAGE_SIZE), 1, 0);

	(page_init - KERNEL_VIRT_OFFSET)(page_directory, page_table1, 1 ,0);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory + (KERNEL_VIRT_OFFSET >> 22), page_table1, 1, 0);

	// Load the page directory to cr3 to tell the cpu to using this page directory
	// to resolve virtual address.
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uint32_t)page_directory));
}
