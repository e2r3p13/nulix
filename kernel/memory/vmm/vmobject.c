// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/vmm/vmoject.h
 *
 * Virtual Memory Object functions
 *
 * created: 2023/01/13 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/17 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/vmm.h>
#include <kernel/memory.h>
#include <kernel/kmalloc.h>

struct vmobject_list vmobject_list = TAILQ_HEAD_INITIALIZER(vmobject_list);

/*
 * Represente a virtual object backed up by physical pages. Can be mapped
 * on virtual space with virtual memory zone.
 * The object is automatically added to the global vmobject_list.
 *
 * NOTE: physical_pages is duplicated.
 */
struct vmobject *vmobject_new_amonymous(physaddr_t physical_pages, size_t size) {
	if (!size)
		return NULL;

	struct vmobject *obj = kmalloc(sizeof(struct vmobject), KMF_NOFAIL);
	if (!obj)
		return NULL;
	obj->physical_pages = physical_pages;
	obj->size = ALIGNNEXT(size, PAGE_SIZE);
	obj->type = VMOBJECT_ANONYMOUS;
	obj->refcount = 0;
	TAILQ_INIT(&obj->vmzone_list);
	TAILQ_INSERT_TAIL(&vmobject_list, obj, next);
	return obj;
}

/*
 * If the number of reference is 0 free the object.
 * The object is automatically removed to the global vmobject_list.
 */
int vmobject_delete(struct vmobject *obj) {
	if (obj->refcount)
		return -1;
	TAILQ_REMOVE(&vmobject_list, obj, next);
	kfree(obj);
	return 0;
}
