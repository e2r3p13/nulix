// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/boot_init.c
 *
 *
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/12 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/paging.h>
#include <kernel/kernel.h>

extern uint32_t sk;
extern uint32_t ek;

#define CLEAR(page, size) \
for (int i = 0; i < PAGE_SIZE; i++) {\
	page[i] = 0; \
}

void boot_init() {
	uint32_t *page_directory = (uint32_t *)0x1000;
	uint32_t *last_page_table = (uint32_t *)0x2000;
	uint32_t *page_table1 = (uint32_t *)0x3000;

	CLEAR(page_directory, PAGE_DIRECTORY_LENGTH);
	page_directory[PAGE_DIRECTORY_LENGTH - 1] = 0x003 | ((uint32_t)last_page_table & (~0xfff));
	CLEAR(last_page_table, PAGE_TABLE_LENGTH);
	last_page_table[PAGE_TABLE_LENGTH - 1] = 0x003 | ((uint32_t)page_directory & (~0xfff));
	CLEAR(page_table1, PAGE_TABLE_LENGTH);

	uint32_t phys_end_kernel = ((uint32_t)&ek - KERNEL_VIRT_OFFSET);
	for (uint32_t pindex = 0; pindex <= PAGE_SIZE; pindex++)
		page_table1[pindex] = (pindex * PAGE_SIZE) | 0x003;

	page_directory[0] = (uint32_t)page_table1 | 0x003;
	last_page_table[0] = (uint32_t)page_table1 | 0x003;

	page_directory[KERNEL_VIRT_OFFSET >> 22] = (uint32_t)page_table1 | 0x003;
	last_page_table[KERNEL_VIRT_OFFSET >> 22] = (uint32_t)page_table1 | 0x003;


	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uint32_t)page_directory));

}
