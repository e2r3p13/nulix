// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/multiboot.h
 *
 * Insert file description here
 *
 * created: 2022/12/08 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/08 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

typedef unsigned char multiboot_uint8_t;
typedef unsigned short multiboot_uint16_t;
typedef unsigned int multiboot_uint32_t;
typedef unsigned long long multiboot_uint64_t;

typedef struct multiboot_info {
	/* Multiboot info version number */
	multiboot_uint32_t flags;

	/* Available memory from BIOS */
	multiboot_uint32_t mem_lower;
	multiboot_uint32_t mem_upper;

	/* "root" partition */
	multiboot_uint32_t boot_device;

	/* Kernel command line */
	multiboot_uint32_t cmdline;

	/* Boot-Module list */
	multiboot_uint32_t mods_count;
	multiboot_uint32_t mods_addr;

	/* Replacing an unused union */
	multiboot_uint32_t pad[4];

	/* Memory Mapping buffer */
	multiboot_uint32_t mmap_length;
	multiboot_uint32_t mmap_addr;

	// Other fields of the multiboot structure are not defined
	// here because they are not needed

} multiboot_info_t;

typedef struct multiboot_mmap_entry {
	multiboot_uint32_t size;
	multiboot_uint64_t addr;
	multiboot_uint64_t len;
	multiboot_uint32_t type;
} __attribute__((packed)) multiboot_memory_map_t;
#endif
