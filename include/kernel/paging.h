// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/paging.h
 *
 *
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/12 - xlmod <glafond-@student.42.fr>
 */

#ifndef PAGING_H
#define PAGING_H

#define PAGE_SIZE 4096

#define PAGE_DIRECTORY_LENGTH	1024
#define PAGE_TABLE_LENGTH		1024

#define LAST_PAGE_ENTRY	0x3ff

void map_page(void *physaddr, void *virtaddr, uint32_t flags);

#endif
