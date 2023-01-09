// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* memory.h
 *
 * Memory related defines, typedefs and functions.
 *
 * created: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
 */

#ifndef MEMORY_H
#define MEMORY_H

// Memory orders
#define KB	1024
#define MB	(KB * KB)
#define GB	(MB * KB)

// Memory alignement
#define ALIGN(size, align)			(((uint32_t)(size)) & (0xffffffff ^ ((align) - 1)))
#define ALIGNNEXT(size, align)		((((uint32_t)(size)) + ((align) - 1)) & (0xffffffff ^ ((align) - 1)))
#define ALIGNNEXTFORCE(size, align)	((((uint32_t)(size)) + ((align))) & (0xffffffff ^ ((align) - 1)))
#define ISALIGNED(size, align)		(!(((uint32_t)(size)) & ((align) - 1)))

// Page size
#define PAGE_SIZE 4096

// Address typedefs
typedef void* physaddr_t;
typedef void* virtaddr_t;

// Physocal Memory mapping
#define PHYSMEM_NULLPAGE_ADDR	0
#define PHYSMEM_KERNEL_ADDR		(1 * MB)

#define PHYSMEM_KMETERNAL_ADDR	(2 * MB)
#define PHYSMEM_KMETERNAL_SIZE	(2 * MB)

#define PHYSMEM_KMFIX_ADDR		(4 * MB)
#define PHYSMEM_KMFIX_SIZE		(4 * MB)

#define PHYSMEM_KMHEAP_ADDR		(8 * MB)

#endif
