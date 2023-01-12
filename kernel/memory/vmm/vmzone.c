// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/vmm.c
 *
 * Virtual Memory Zone functions
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/12 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/vmm.h>
#include <kernel/memory.h>
#include <kernel/paging.h>
#include <kernel/kmalloc.h>
#include <kernel/kpm.h>
#include <kernel/string.h>

/*
 *	Allocate a vmzone struct and fill it with given data
 *
 *	INFO: name and chunk_list are duplicated.
 */
struct vmzone *vmzone_map(virtaddr_t start, virtaddr_t end, char *name,
		uint32_t flags, struct kpm_chunk_head *chunk_list) {
	if (start >= end)
		return VIRTADDR(NULL);
	if (!ISALIGNED(start, PAGE_SIZE) || !ISALIGNED(end, PAGE_SIZE))
		return VIRTADDR(NULL);
	if (!(*(struct page_entry_flags *)&flags).present)
		return VIRTADDR(NULL);


	struct vmzone *newzone = kmalloc(sizeof(struct vmzone), KMF_NOFAIL);
	if (!newzone)
		return VIRTADDR(NULL);

	newzone->name = kmalloc(sizeof(char) * (strlen(name) + 1), KMF_NOFAIL);
	if (!newzone->name) {
		kfree(newzone);
		return VIRTADDR(NULL);
	}
	strcpy(newzone->name, name);

	struct kpm_chunk *chunk, *tmp;
	TAILQ_FOREACH(chunk, chunk_list, next) {
		tmp = kmalloc(sizeof(struct kpm_chunk), KMF_NOFAIL);
		if (!tmp) {
			vmzone_free(newzone);
			return VIRTADDR(NULL);
		}
		TAILQ_INSERT_TAIL(&newzone->chunk_list, tmp, next);
	}

	newzone->start = start;
	newzone->end = end;
	newzone->flags = *(struct page_entry_flags *)&flags;

	return VIRTADDR(newzone);
}

/*
 * Free the name, the chunk_list and the ptr of a vmzone
 */
void vmzone_free(struct vmzone *vmzone) {
	struct kpm_chunk *chunk;
	TAILQ_FOREACH(chunk, &vmzone->chunk_list, next)
		kfree(chunk);
	kfree(vmzone->name);
	kfree(vmzone);
}
