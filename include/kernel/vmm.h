// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/vmm.h
 *
 * Virtual Memory Manager header
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/12 - glafond- <glafond-@student.42.fr>
 */

#ifndef VMM_H
#define VMM_H

#include <kernel/memory.h>
#include <kernel/paging.h>
#include <kernel/kpm.h>
#include <kernel/symbole.h>

#define VM_QUICKMAP_ADDR (VIRTADDR(SYM_STACK_TOP))

struct vmzone {
	virtaddr_t start;
	virtaddr_t end;
	char *name;
	struct page_entry_flags flags;
	TAILQ_HEAD(,kpm_chunk) chunk_list;
	TAILQ_ENTRY(vmzone) next;
};

struct vmmap {
	physaddr_t pagedir;
	TAILQ_HEAD(,vmzone)	vmzone_list;
};

int map_page(virtaddr_t vaddr, physaddr_t paddr, uint32_t flags);
int map_zone(virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags);
int map_page_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, uint32_t flags);
int map_zone_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags);

virtaddr_t quickmap_page(physaddr_t addr);

struct vmzone *vmzone_new(virtaddr_t start, virtaddr_t end, char *name,
		uint32_t flags, struct kpm_chunk_head *chunk_list);
void vmzone_free(struct vmzone *vmzone);

struct vmmap *vmmap_new(physaddr_t pagedir);

int vmmap_map_zone(struct vmzone *vmzone);
int vmmap_unmap_zone(struct vmzone *vmzone);

struct vmzone *vmmap_find_zone_by_name(char *name);
struct vmzone *vmmap_find_zone_by_addr(virtaddr_t addr);

#endif
