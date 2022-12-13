// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/kvm.c
 *
 * Virtual memory manager
 *
 * created: 2022/12/13 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/16 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/kvm.h>
#include <kernel/paging.h>
#include <kernel/kpm.h>
#include <kernel/string.h>

/*
 * Creates and sets the kernel page directory,
 * with mapped regions
 */
void kvm_init() {
	struct page_entry *kernel_pagedir;

	kernel_pagedir = kvm_new_pagedirectory();
	if (!kernel_pagedir)
		return ; // TODO: better way to handler alloc error (stop the kernel, etc)
	
	//TODO : map the kernel, the gdt and the buddy allocator

	kvm_load_cr3(kernel_pagedir);

}

/*
 * Creates a new page directory
 * Returns a pointer to the newly created pagedir struct
 */
struct page_entry *kvm_new_pagedirectory() {
	kpm_chunk_t	mem_chunk;
	struct page_entry *pagedir;

	if (kpm_alloc(&mem_chunk, PAGE_SIZE))
		return NULL;

	memset(mem_chunk.addr, 0, mem_chunk.size);
	pagedir = (struct page_entry *)mem_chunk.addr;
	page_init(pagedir + LAST_PAGE_ENTRY, pagedir, 1, 0);

	return pagedir;
}

/*
 * Sets the CR3 register to set the given page directory as
 * the used one
 */
void kvm_load_cr3(struct page_entry *pagedir) {
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uintptr_t)pagedir));
}

struct page_entry *kvm_get_cr3() {
	struct page_entry *pagedir;
	__asm__ volatile ("movl %%cr3, %0" :: "r" ((uintptr_t)pagedir));
	return pagedir;
}

void kvm_reload_cr3() {
	kvm_load_cr3(kvm_get_cr3());
}

/*
 * Maps @n pages from @phy to @virt virtual addresses.
 * NOTE: if @phy isn't aligned the whole page is mapped.
 */
struct page_entry *kvm_map(void *phy, void *virt) {
	uint32_t pagedir_index = (uint32_t)virt >> 22;
	uint32_t pagetab_index = ((uint32_t)virt >> 12) & (PAGE_TABLE_LENGTH - 1);

	struct page_entry *pd = (struct page_entry *)0xFFFFF000;
	if (!pd[pagedir_index].present) {
		// TODO: page table not present need to alloc a new page table add it to
		// the page directory, then continue.
	}

	struct page_entry *pt = (struct page_entry *)(0xFFC00000 + (pagedir_index << 12));
	page_init(pt + pagetab_index, phy, 0, 0);
	return pt + pagetab_index;
}

/*
 * Unmaps @n pages from @virt address
 */
void kvm_unmap(void *virt, uint32_t n);

/*
 * Allocates @size bytes through kpm_alloc and
 * returns the virtual address of the newly allocated
 * area
 */
void *kvm_alloc(uint32_t size);

/*
 * Releases memory
 * Note that @addr must be page aligned, and should have
 * been obtained through a previous kvm_alloc call
 * @size determines the size of the memory area to release
 */
void kvm_free(void *addr, uint32_t size);

/*
 * Converts a virtual address to a physical one
 */
static void *get_physical(void *virt);

