// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/paging.h
 *
 * Header for pagination related things
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/13 - glafond- <glafond-@student.42.fr>
 */

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <kernel/memory.h>

#define PAGE_DIR_LENGTH	1024
#define PAGE_TABLE_LENGTH		PAGE_DIR_LENGTH

#define PAGE_DIR_MASK			0xffc00000 // 0b11111111110000000000000000000000
#define PAGE_TAB_MASK			0x00bff000 // 0b00000000001111111111000000000000
#define PAGE_INDEX_MASK			0x00000fff // 0b00000000000000000000111111111111

#define PAGE_DIR_GET_INDEX(v)	(((uint32_t)(v) & PAGE_DIR_MASK) >> 22)
#define PAGE_TAB_GET_INDEX(v)	(((uint32_t)(v) & PAGE_TAB_MASK) >> 12)
#define PAGE_GET_INDEX(v)		((uint32_t)(v) & PAGE_INDEX_MASK)

#define LAST_PAGE_ENTRY			(PAGE_DIR_LENGTH - 1)

struct page_entry {
	uint32_t present: 1;
	uint32_t writable: 1;
	uint32_t user: 1;
	uint32_t write_through: 1;
	uint32_t cache_disable: 1;
	uint32_t accessed: 1;
	uint32_t specific: 3;
	uint32_t avl_3: 1;
	uint32_t avl_4: 1;
	uint32_t avl_5: 1;
	uint32_t address: 20;
};

struct page_directory_entry {
	uint32_t present: 1;
	uint32_t writable: 1;
	uint32_t user: 1;
	uint32_t write_through: 1;
	uint32_t cache_disable: 1;
	uint32_t accessed: 1;
	uint32_t avl_1: 1;
	uint32_t page_size: 1;
	uint32_t avl_2: 1;
	uint32_t avl_3: 1;
	uint32_t avl_4: 1;
	uint32_t avl_5: 1;
	uint32_t address: 20;
};

struct page_table_entry {
	uint32_t present: 1;
	uint32_t writable: 1;
	uint32_t user: 1;
	uint32_t write_through: 1;
	uint32_t cache_disable: 1;
	uint32_t accessed: 1;
	uint32_t dirty: 1;
	uint32_t page_attribute_table: 1;
	uint32_t global: 1;
	uint32_t avl_3: 1;
	uint32_t avl_4: 1;
	uint32_t avl_5: 1;
	uint32_t address: 20;
};

struct page_entry_flags {
	uint32_t present: 1;
	uint32_t writable: 1;
	uint32_t user: 1;
	uint32_t write_through: 1;
	uint32_t cache_disable: 1;
	uint32_t accessed: 1;
	uint32_t pad: 26;
};

void page_init(struct page_entry *pe, void *addr, int w, int u);
void page_clear(struct page_entry *pe);

physaddr_t get_cr3();
void set_cr3(physaddr_t addr);
void reload_cr3();

#endif
