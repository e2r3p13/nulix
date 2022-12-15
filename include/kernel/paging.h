// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/paging.h
 *
 * Header for pagination related things
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - glafond- <glafond-@student.42.fr>
 */

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 4096

#define PAGE_DIRECORY_LENGTH	(PAGE_SIZE / sizeof(uint32_t))
#define PAGE_TABLE_LENGTH		PAGE_DIRECORY_LENGTH

#define LAST_PAGE_ENTRY			(PAGE_DIRECORY_LENGTH - 1)

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

#endif
