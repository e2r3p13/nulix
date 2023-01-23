// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/paging.h
 *
 * Header for pagination related things
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
 */

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <kernel/memory.h>

#define PAGE_DIRECTORY_LENGTH	1024
#define PAGE_TABLE_LENGTH		PAGE_DIRECTORY_LENGTH

#define LAST_PAGE_ENTRY			(PAGE_DIRECTORY_LENGTH - 1)

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

/*
 * Initialize a page entry with given flags
 */
void page_init(struct page_entry *pe, void *addr, int w, int u);

/*
 * Clear the page entry
 */
void page_clear(struct page_entry *pe);

#endif
