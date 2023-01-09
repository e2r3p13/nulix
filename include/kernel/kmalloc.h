// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/kmalloc.h
 *
 * Kmalloc header
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - xlmod <glafond-@student.42.fr>
 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>

#include <kernel/bitmaptree.h>
#include <kernel/list.h>
#include <kernel/kpm.h>
#include <kernel/slab.h>

#define KMALLOC_MEMORY_ALIGN 16

struct kmalloc_node {
	void *address;
	size_t size;
	TAILQ_ENTRY(kmalloc_node) next;
};

#define KM_BMT_NLAYERS 8

struct kmalloc_fix {
	virtaddr_t start;
	virtaddr_t end;
	struct bitmaptree kbmt;
	TAILQ_HEAD(,kmalloc_node) nodes_list;
	struct slab node_slab;
	struct kpm_chunk pchunk;
};

struct kmalloc_block {
	virtaddr_t baseaddr;
	struct bitmaptree kbmt;
	TAILQ_HEAD(,kpm_chunk) pchunks;
	TAILQ_ENTRY(kmalloc_block) next;
};

struct kmalloc_heap {
	struct bitmap blocks;
	SLIST_HEAD(,kmalloc_block) blocks_list;
	SLIST_HEAD(,kmalloc_node) nodes_list;
	virtaddr_t start;
	virtaddr_t end;
};

struct kmalloc_eternal {
	virtaddr_t start;
	virtaddr_t end;
	virtaddr_t cur;
	struct kpm_chunk pchunk;
};

struct kmalloc {
	struct kmalloc_fix fix;
	struct kmalloc_heap heap;
	struct kmalloc_eternal eternal;
};

#define KMZ_NTYPE		3
#define KMZ_FIX			0
#define KMZ_HEAP		1
#define KMZ_ETERNAL		2

int kmalloc_eternal_init();
int kmalloc_fix_init();
int kmalloc_heap_init();

void *kmalloc(size_t size, int type);
void kfree(void *addr);

void *krealloc(void *addr, size_t size);
void *kzalloc(size_t size, int type);


#endif
