// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/mapping.c
 *
 *
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/12 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>

void map_page(void *physaddr, void *virtaddr, uint32_t flags) {
	uint32_t pagedir_index = (uint32_t)virtaddr >> 22;
	uint32_t pagetab_index = ((uint32_t)virtaddr >> 12) & (PAGE_TABLE_LENGTH - 1);

	uint32_t *pd = (uint32_t *)(((LAST_PAGE_ENTRY << 10) | LAST_PAGE_ENTRY) << 12);
	uint32_t *pt = ((uint32_t *)LAST_PAGE_ENTRY << 22) + (PAGE_TABLE_LENGTH * pagedir_index);

	pt[pagetab_index] = ((uint32_t)physaddr) | (flags & (PAGE_SIZE - 1)) | 0x01;
}
