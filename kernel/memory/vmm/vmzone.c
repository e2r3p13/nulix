// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/vmm.c
 *
 * Virtual Memory Zone functions
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/17 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/vmm.h>
#include <kernel/memory.h>
#include <kernel/paging.h>
#include <kernel/kmalloc.h>
#include <kernel/kpm.h>
#include <kernel/string.h>
#include <kernel/spinlock.h>

/*
 *	Allocate a vmzone struct and fill it with given data
 *
 *	INFO: name and chunk_list are duplicated.
 */
struct vmzone *vmzone_new(virtaddr_t start, virtaddr_t end, char *name,
		uint32_t flags, struct vmobject *obj, size_t offset) {
	if (start >= end)
		return NULL;
	if (!ISALIGNED(start, PAGE_SIZE))
		return NULL;
	if (!(*(struct page_entry_flags *)&flags).present)
		return NULL;
	if ((offset + (end - start)) / PAGE_SIZE > obj->size / PAGE_SIZE)
		return NULL;

	struct vmzone *newzone = kmalloc(sizeof(struct vmzone), KMF_NOFAIL);
	if (!newzone)
		return NULL;

	newzone->name = kmalloc(sizeof(char) * (strlen(name) + 1), KMF_NOFAIL);
	if (!newzone->name) {
		kfree(newzone);
		return NULL;
	}
	strcpy(newzone->name, name);

	newzone->start = start;
	newzone->end = VIRTADDR(ALIGNNEXT(end, PAGE_SIZE));
	newzone->flags = *(struct page_entry_flags *)&flags;
	newzone->offset = offset;

	newzone->obj = obj;
	kspin_lock(&newzone->obj->lock);
	newzone->obj->refcount++;
	kspin_drop(&newzone->obj->lock);

	return newzone;
}

/*
 * Free the name, the chunk_list and the ptr of a vmzone
 */
void vmzone_delete(struct vmzone *vmzone) {
	kspin_lock(&vmzone->obj->lock);
	if (vmzone->obj->refcount)
		vmzone->obj->refcount--;
	kspin_drop(&vmzone->obj->lock);

	kfree(vmzone->name);
	kfree(vmzone);
}
