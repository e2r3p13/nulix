// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/vmm/mmap.c
 *
 * Mapping functions
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/12 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/vmm.h>
#include <kernel/paging.h>
#include <kernel/kmalloc.h>

/*
 * Map a physical page on a virtual address to the loaded page directory.
 */
int map_page(virtaddr_t vaddr, physaddr_t paddr, uint32_t flags) {
	uint32_t pdir_index = PAGE_DIR_GET_INDEX(vaddr);
	uint32_t ptab_index = PAGE_TAB_GET_INDEX(vaddr);

	struct page_entry *pdir = (struct page_entry *)0xFFFFFF000;
	if (!pdir[pdir_index].present) {
		physaddr_t newtab = kpm_alloc_page();
		if (!newtab)
			return -1;
		page_init(pdir + pdir_index, newtab, 1, 0);
	}
	struct page_entry *ptab = (struct page_entry *)(0xFFC00000 + (pdir_index << 12));
	struct page_entry *page = ptab + ptab_index;
	struct page_entry_flags *pflags = (struct page_entry_flags *)page;
	page_init(page, paddr, 0, 0);
	*pflags |= flags;
	return 0;
}

/*
 * Map consecutive physical pages on a virtual address to the loaded page directory.
 */
int map_zone(virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags) {
	for (size_t n = 0; n < npages; n++) {
		size_t offset = n * PAGE_SIZE;
		if (map_page(vaddr + offset, paddr + offset, flags) < 0)
			return -1;
	}
	return 0;
}

/*
 * Map a physical page on a virtual address to a given page directory.
 */
int map_page_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, uint32_t flags) {
	uint32_t pdir_index = PAGE_DIR_GET_INDEX(vaddr);
	uint32_t ptab_index = PAGE_TAB_GET_INDEX(vaddr);

	struct page_entry *pdir = (struct page_entry *)quickmap_page(pagedir);
	if (!pdir)
		return -1;
	physaddr_t newtab;
	if (!pdir[pdir_index].present) {
		newtab = kpm_alloc_page();
		if (!newtab)
			return -1;
		page_init(pdir + pdir_index, newtab, 1, 0);
	}
	struct page_entry *ptab = (struct page_entry *)quickmap_page(newtab);
	struct page_entry *page = ptab + ptab_index;
	struct page_entry_flags *pflags = (struct page_entry_flags *)page;
	page_init(page, paddr, 0, 0);
	*pflags |= flags;
	return 0;
}

/*
 * Map consecutive physical pages on a virtual address to a given page directory.
 */
int map_zone_to(physaddr_t pagedir, virtaddr_t vaddr, physaddr_t paddr, size_t npages, uint32_t flags) {
	for (size_t n = 0; n < npages; n++) {
		size_t offset = n * PAGE_SIZE;
		if (map_page_to(pagedir, vaddr + offset, paddr + offset, flags) < 0)
			return -1;
	}
	return 0;
}

/*
 *	Map a  physical page to a reserved virtual zone. Each call to quickmap_page
 *	overrides the previous mapping.
 */
virtaddr_t quickmap_page(physaddr_t addr) {
	struct page_entry_flags flags;
	flags.present = 1;
	flags.writable = 1;
	if (map_page(VM_QUICKMAP_ADDR, addr, (uint32_t)flags) < 0)
		return VIRTADDR(NULL);
	return VM_QUICKMAP_ADDR;
}
