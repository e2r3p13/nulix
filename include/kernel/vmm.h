// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/vmm.h
 *
 * Virtual Memory Manager header
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/13 - glafond- <glafond-@student.42.fr>
 */

#ifndef VMM_H
#define VMM_H

#include <kernel/memory.h>
#include <kernel/paging.h>
#include <kernel/kpm.h>
#include <kernel/symbole.h>
#include <kernel/spinlock.h>

extern struct vmobject_list vmobject_list; 
extern struct vmmap_list vmmap_list; 

#define VM_QUICKMAP_ADDR (VIRTADDR(STACK_TOP))

enum vmobject_type {
	VMOBJECT_ANONYMOUS,
};

struct vmobject {
	spinlock_t lock;
	enum vmobject_type type;
	physaddr_t physical_pages;
	size_t size;
	TAILQ_HEAD(,vmzone) vmzone_list;
	TAILQ_ENTRY(vmobject) next;
	uint16_t refcount;
};

TAILQ_HEAD(vmobject_list, vmobject);

struct vmzone {
	virtaddr_t start;
	virtaddr_t end;
	char *name;
	struct vmobject *obj;
	size_t offset;
	struct page_entry_flags flags;
	TAILQ_ENTRY(vmzone) next;
};

struct vmmap {
	physaddr_t pagedir;
	TAILQ_HEAD(vmzone_list,vmzone)	vmzone_list;
};

TAILQ_HEAD(vmmap_list, vmmap);

int map_page(virtaddr_t vaddr, physaddr_t paddr, uint32_t flags);
int unmap_page(virtaddr_t vaddr);
int map_zone(virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags);
int unmap_zone(virtaddr_t vaddr, size_t npages);

int map_page_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, uint32_t flags);
int unmap_page_to(physaddr_t pagedir, virtaddr_t vaddr);
int map_zone_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags);
int unmap_zone_to(physaddr_t pagedir, virtaddr_t vaddr, size_t npages);

virtaddr_t quickmap_page(physaddr_t addr);

struct vmobject *vmobject_new_amonymous(physaddr_t physical_pages, size_t npages);
int vmobject_delete(struct vmobject *obj);

struct vmzone *vmzone_new(virtaddr_t start, virtaddr_t end, char *name,
		uint32_t flags, struct vmobject *obj, size_t offset);
void vmzone_free(struct vmzone *vmzone);

struct vmmap *vmmap_new(physaddr_t pagedir);

int vmmap_map_zone(struct vmmap *map, struct vmzone *vmzone);
int vmmap_unmap_zone(struct vmmap *map, struct vmzone *vmzone);

struct vmzone *vmmap_find_zone_by_name(struct vmmap *map, char *name);
struct vmzone *vmmap_find_zone_by_addr(struct vmmap *map, virtaddr_t addr);

#endif
