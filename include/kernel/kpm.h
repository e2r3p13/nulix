// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kpm.h
 *
 * Kernel Physical Memory management header file
 *
 * created: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/05 - glafond- <glafond-@student.42.fr>
 */

#ifndef KPM_H
#define KPM_H

#include <stddef.h>
#include <stdint.h>

#include <kernel/multiboot.h>
#include <kernel/list.h>
#include <kernel/bitmap.h>

#define PAGE_SIZE		4096

#define ALIGN(size, align)			(((uint32_t)(size)) & (0xffffffff ^ ((align) - 1)))
#define ALIGNNEXT(size, align)		((((uint32_t)(size)) + ((align) - 1)) & (0xffffffff ^ ((align) - 1)))
#define ALIGNNEXTFORCE(size, align)	((((uint32_t)(size)) + ((align))) & (0xffffffff ^ ((align) - 1)))
#define ISALIGNED(size, align)		(!(((uint32_t)(size)) & ((align) - 1)))

#define KPM_NORDERS		11

/*
 * Represents a page frame.
 */

#define KPM_NBYTES_FROM_NBITS(n)		(ALIGNNEXT(n, 8) / 8)

#define KPM_ALLOC(order, index)			(buddy->orders[order].bitmap[(index)/8] |= (1 << ((index) % 8)))
#define KPM_FREE(order, index)			(buddy->orders[order].bitmap[(index)/8] &= ~(1 << ((index) % 8)))
#define KPM_GET(order, index)			(buddy->orders[order].bitmap[(index)/8] & (1 << ((index) % 8)))
#define KPM_IS_ALLOCATED(order, index)	(KPM_GET(order, index) != 0)

#define KPM_ENABLE(index)				(buddy->enabled_frames[(index)/8] |= (1 << ((index) % 8)))
#define KPM_DISABLE(index)				(buddy->enabled_frames[(index)/8] &= ~(1 << ((index) % 8)))
#define KPM_IS_ENABLED(index)			((buddy->enabled_frames[(index)/8] & (1 << ((index) % 8))) != 0)

/*
 * The buddy allocator structure, that contains 11 orders.
 * Each order is an array of page frame of different size.
 * Their size depends of the order, next order have page frame twice bigger
 * than the previous.
 *
 * @nareas is the number of page frame of the smallest size (KB).
 * @orders is the pointer to the array of orders.
 */
typedef struct buddy {
	size_t nframes;
	size_t nfree;
	size_t size;
	struct bitmap enabled_frames;
	struct bitmap orders[KPM_NORDERS];
} buddy_t;

/*
 * This function describes a memory region allocated by
 * kpm_alloc.
 *
 * @addr is the base address of the allocated chunk
 * @size is the size in bytes of the allocated chunk
 */
typedef struct kpm_chunk {
	void *addr;
	size_t size;
	TAILQ_ENTRY(kpm_chunk) list;
} kpm_chunk_t;

TAILQ_HEAD(kpm_chunk_head, kpm_chunk);

/*
 * Creates and itinialize the buddy allocator with memory maps information.
 * Our buddy allocator contains 11 levels, allowing for allocations from
 * 4KB to 4MB.
 *
 * Internally stores the address of the buddy structure
 */
void kpm_init(struct multiboot_mmap_entry *mmap_entries, size_t n, size_t memsize);

/*
 * Enables or disable memory regison to make them available or not
 * for later kpm_alloc/free calls.
 */
void kpm_enable(void *base, size_t limit);
void kpm_disable(void *base, size_t limit);

/*
 * Returns 1 if the memory region starting at @addr is enabled/allocated,
 * 0 if it's free.
 */
int kpm_isenabled(void *addr);
int kpm_isalloc(void *addr);

/*
 * Allocate @size bytes of memory
 * Returns the address of the newly allocated region, or NULL on error
 */
//int kpm_alloc(kpm_chunk_t *chunk, size_t size);
int kpm_alloc(struct kpm_chunk_head *head, size_t size);

/*
 * Release the buddy node starting at addr @addr
 */
//void kpm_free(kpm_chunk_t *chunk);
void kpm_free(struct kpm_chunk_head *head);

/*
 * Prints the buddy allocator
 */
void buddy_print();

#endif
