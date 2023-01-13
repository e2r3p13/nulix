// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/vmm/vmmap.c
 *
 * Virtual Memory Mapping functions
 *
 * created: 2023/01/12 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/13 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/vmm.h>
#include <kernel/paging.h>
#include <kernel/kmalloc.h>
#include <kernel/string.h>

struct vmmap_list vmmap_list; 

/*
 * Return a pointer to an allocated vmmap.
 */
struct vmmap *vmmap_new(physaddr_t pagedir) {
	struct vmmap *map = kmalloc(sizeof(struct vmmap), KMF_NOFAIL);
	if (!map)
		return NULL;
	map->pagedir = pagedir;
	TAILQ_INIT(&map->vmzone_list);
	return map;
}

/*
 * Map the vmzone in the page directory of vmmap.
 */
int vmmap_map_zone(struct vmmap *map, struct vmzone *zone) {
	int loaded = map->pagedir == get_cr3();

	struct vmzone *ptr = TAILQ_FIRST(&map->vmzone_list);
	while (ptr && zone->end >= ptr->start)
		ptr = TAILQ_NEXT(ptr, next);
	if (!ptr)
		ptr = TAILQ_LAST(&map->vmzone_list, vmzone_list);
	else
		ptr = TAILQ_PREV(ptr, vmzone_list, next);
	if (zone->start < ptr->end)
		return -1;
	TAILQ_INSERT_AFTER(&map->vmzone_list, ptr, zone, next);
	
	size_t npages = (zone->end - zone->start) / PAGE_SIZE;
	if (loaded) {
		if (map_zone(zone->start, zone->obj->physical_pages + zone->offset,
					npages, *(uint32_t *)&zone->flags) < 0)
			return -1;
	} else {
		if (map_zone_to(map->pagedir, zone->start, zone->obj->physical_pages + zone->offset,
					npages, *(uint32_t *)&zone->flags) < 0)
			return -1;
	}
	return 0;
}

/*
 * Unmap a mapped zone.
 */
int vmmap_unmap_zone(struct vmmap *map, struct vmzone *zone) {
	struct vmzone *ptr = vmmap_find_zone_by_addr(map, zone->start);
	if (!ptr)
		return -1;
	int loaded = map->pagedir == get_cr3();
	TAILQ_REMOVE(&map->vmzone_list, zone, next);
	size_t npages = (zone->end - zone->start) / PAGE_SIZE;
	if (loaded)
		unmap_zone(zone->start, npages);
	else
		unmap_zone_to(map->pagedir, zone->start, npages);
	return 0;
}

/*
 * Return a pointer to a zone named @name. Otherwise return NULL.
 */
struct vmzone *vmmap_find_zone_by_name(struct vmmap *map, char *name) {
	struct vmzone *ptr;
	TAILQ_FOREACH(ptr, &map->vmzone_list, next) {
		if (strcmp(name, ptr->name) == 0)
			return ptr;
	}
	return NULL;
}

/*
 * Return a pointer to a zone containing @addr. Otherwise return NULL.
 */
struct vmzone *vmmap_find_zone_by_addr(struct vmmap *map, virtaddr_t addr) {
	struct vmzone *ptr;
	TAILQ_FOREACH(ptr, &map->vmzone_list, next) {
		if (addr >= ptr->start && addr < ptr->end)
			return ptr;
	}
	return NULL;
}

