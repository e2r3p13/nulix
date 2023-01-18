// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kmalloc_internal.h
 *
 * Kmalloc internal header
 *
 * created: 2023/01/10 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#ifndef KMALLOC_INTERNAL_H
#define KMALLOC_INTERNAL_H

#include <stddef.h>
#include <kernel/bitmaptree.h>
#include <kernel/list.h>
#include <kernel/spinlock.h>

#define KM_MEMORY_CHUNK_SIZE	16
#define KM_INITIAL_SIZE		(2 * MB)

#define KM_BMT_NLAYERS	14
#define KM_BMT_SIZE_ARRAY (((KM_INITIAL_SIZE / KM_BMT_NLAYERS) / 8) * 2)

struct km_header {
	size_t	allocated_size;
	uint8_t	pad[8];
	uint32_t crc;
	uint8_t data[0];
};

struct km_block {
	virtaddr_t vaddr;
	physaddr_t paddr;
	size_t size;
	size_t nchunk;
	size_t nallocated;
	struct bitmaptree bmt;
	TAILQ_ENTRY(km_block) next;
};

struct kmalloc {
	spinlock_t lock;
	size_t nblock;
	TAILQ_HEAD(,km_block) block_list;
};

#endif
