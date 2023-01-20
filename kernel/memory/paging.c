// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/paging.c
 *
 * Paging related functions
 * 
 * created: 2022/12/16 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/20 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/paging.h>
#include <kernel/vmm.h>
#include <kernel/symbole.h>
#include <kernel/kernel.h>
#include <kernel/string.h>

/*
 * Initialize a page entry with given flags
 */
void page_init(struct page_entry *pe, void *addr, int w, int u) {
	*(uintptr_t *)pe = 0;
	pe->present = 1;
	pe->writable = w;
	pe->user = u;
	pe->address = ((uintptr_t)addr >> 12); 
}

/*
 * Clear the page entry
 */
void page_clear(struct page_entry *pe) {
	*(uintptr_t *)pe = 0;
}

/*
 * Return the physical address of a virtual address.
 */
physaddr_t get_physaddr(virtaddr_t vaddr) {
	vaddr = VIRTADDR(ALIGN(vaddr, PAGE_SIZE));
	uint32_t pdir_index = PAGE_DIR_GET_INDEX(vaddr);
	uint32_t ptab_index = PAGE_TAB_GET_INDEX(vaddr);
	struct page_entry *pdir = (struct page_entry *)0xFFFFFF000;
	if (!pdir[pdir_index].present)
		return PHYSADDR(NULL);
	struct page_entry *ptab = (struct page_entry *)(0xFFC00000 + (pdir_index  << 12));
	struct page_entry *page = &ptab[ptab_index];
	return PHYSADDR(page->address << 12);
}

/*
 * Return the address loaded in cr3
 */
inline physaddr_t get_cr3() {
	physaddr_t addr;
	__asm__ volatile ("movl %%cr3, %0" : "=r" ((uintptr_t)addr));
	return addr;
}

/*
 * Set an address in cr3
 */
inline void set_cr3(physaddr_t addr) {
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uintptr_t)addr));
}

/*
 * Reload the cr3 to flush the tlb
 */
inline void reload_cr3() {
	set_cr3(get_cr3());
}

/*
 * Create a page directory for the kernel and map it right.
 */
physaddr_t page_directory_kernel_new() {

	physaddr_t pagedir = kpm_alloc_page();
	if (!pagedir)
		return PHYSADDR(NULL);
	void *pdiraddr = quickmap_page(pagedir);
	memset(pdiraddr, 0, 0x1000);
	page_init((struct page_entry *)pdiraddr + LAST_PAGE_ENTRY, pagedir, 1, 0);
	map_page_to(pagedir, VIRTADDR(0xb8000 + KERNEL_VIRT_OFFSET), PHYSADDR(0xb8000), 0x003);

	size_t size = SYM_KERNEL_END - SYM_KERNEL_START;
	struct vmobject *obj = vmobject_new_amonymous(PHYSADDR(SYM_KERNEL_START), size);
	if (!obj) {
		kpm_free_page(pagedir);
		return PHYSADDR(NULL);
	}

	struct vmmap *map = vmmap_new(pagedir);
	if (!map) {
		vmobject_delete(obj);
		kpm_free_page(pagedir);
		return PHYSADDR(NULL);
	}

	struct vmzone *zone;
	struct page_entry_flags flags;
	*(uint32_t *)&flags = 0;
	flags.present = 1;
	zone = vmzone_new(VIRTADDR(SYM_VIRT_TEXT), VIRTADDR(SYM_VIRT_TEXT + SYM_TEXT_SIZE),
			".text", *(uint32_t *)&flags, obj, PHYSADDR(SYM_TEXT) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	zone = vmzone_new(VIRTADDR(SYM_VIRT_RODATA), VIRTADDR(SYM_VIRT_RODATA + SYM_RODATA_SIZE),
			".rodata", *(uint32_t *)&flags, obj, PHYSADDR(SYM_RODATA) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	flags.writable = 1;
	zone = vmzone_new(VIRTADDR(SYM_VIRT_DATA), VIRTADDR(SYM_VIRT_DATA + SYM_DATA_SIZE),
			".data", *(uint32_t *)&flags, obj, PHYSADDR(SYM_DATA) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	zone = vmzone_new(VIRTADDR(SYM_VIRT_BSS), VIRTADDR(SYM_VIRT_BSS + SYM_BSS_SIZE),
			".bss", *(uint32_t *)&flags, obj, PHYSADDR(SYM_BSS) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	zone = vmzone_new(VIRTADDR(SYM_VIRT_BUDDY), VIRTADDR(SYM_VIRT_BUDDY + SYM_BUDDY_SIZE),
			".buddy", *(uint32_t *)&flags, obj, PHYSADDR(SYM_BUDDY) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	zone = vmzone_new(VIRTADDR(SYM_VIRT_HEAP), VIRTADDR(SYM_VIRT_HEAP + SYM_HEAP_SIZE),
			".heap", *(uint32_t *)&flags, obj, PHYSADDR(SYM_HEAP) - obj->physical_pages); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);

	physaddr_t stack = kpm_alloc_zone(SYM_STACK_SIZE);
	if (!stack) {
		vmmap_delete(map);
		vmobject_delete(obj);
		return PHYSADDR(NULL);
	}
	size =  SYM_STACK_SIZE;
	struct vmobject *objstack = vmobject_new_amonymous(stack, size);
	if (!obj) {
		vmmap_delete(map);
		vmobject_delete(obj);
		kpm_free_page(stack);
		return PHYSADDR(NULL);
	}
	zone = vmzone_new(VIRTADDR(SYM_VIRT_STACK), VIRTADDR(SYM_VIRT_STACK + SYM_STACK_SIZE),
			".stack", *(uint32_t *)&flags, objstack, 0); 
	if (!zone) {
		vmmap_delete(map);
		vmobject_delete(obj);
		vmobject_delete(objstack);
		kpm_free_page(stack);
		return PHYSADDR(NULL);
	}
	vmmap_map_zone(map, zone);
	
	return pagedir;
}
