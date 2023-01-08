// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/alloc.c
 *
 * Allocate builtin file
 *
 * created: 2022/12/13 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/stdlib.h>
#include <kernel/list.h>
#include <kernel/bitmap.h>
#include <kernel/string.h>

#define BLTNAME "alloc"

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

static inline void usage() {
	kprintf("Usage: " BLTNAME " size\n");
}

#define MAXALLOC 10
struct kpm_chunk_head chunk_heads[MAXALLOC] = {0};
uint8_t alloc_bitmap_array[((MAXALLOC - 1) / 8) + 1];
struct bitmap alloc_bitmap;
int alloc_bitmap_init = 0;

void alloc_print() {
	struct kpm_chunk *chunk;
	int index = 0;
	kprintf("Allocation list:\n");
	while ((index = bitmap_get_next_one(&alloc_bitmap, index)) != -1) {
		kprintf("Allocation %d/%d\n", index, MAXALLOC);
		uint8_t oldcolor = sb_get_color(sb_current);
		sb_set_fg(sb_current, SB_COLOR_GREEN);
		TAILQ_FOREACH(chunk, &chunk_heads[index], list) {
			kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", chunk->addr, chunk->size);
		}
		sb_set_color(sb_current, oldcolor);
		index++;
	}
}


/*
 * Implements the alloc buitin, that takes
 * a size and wraps make needed calls to
 * kpm_alloc to get enough memory.
 * Prints information about each allocated
 * node.
 * Returns 0 if successful, -1 if an error
 * occured.
 */
int alloc(int argc, char **argv) {
	if (alloc_bitmap_init == 0) {
		alloc_bitmap_init = 1;
		bitmap_init(&alloc_bitmap, MAXALLOC, alloc_bitmap_array, ((MAXALLOC - 1) / 8) + 1);
	}

	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	int32_t size = strtol(argv[1], &ptr, 0);
	if (*ptr != 0 && !strcmp(argv[1], "print")) {
		alloc_print();
		return 0;
	} else if (*ptr != 0 || size < 0) {
		kprintf(BLTNAME ": Size not well formatted.\n");
		return -1;
	}
	int index = bitmap_get_first_zero(&alloc_bitmap);
	if (index < 0) {
		kprintf(BLTNAME ": Max allocation reached (%d).\n", MAXALLOC);
		return -1;
	}
	kprintf("Asked for allocation of %d bytes\n", size);
	struct kpm_chunk *chunk;
	if (kpm_alloc(&chunk_heads[index], size) < 0) {
		kprintf("Failed to allocate memory area.\n");
		return -1;
	}
	bitmap_set_at(&alloc_bitmap, index, 1);
	kprintf("Allocation %d/%d\n", index, MAXALLOC);
	uint8_t oldcolor = sb_get_color(sb_current);
	sb_set_fg(sb_current, SB_COLOR_GREEN);
	TAILQ_FOREACH(chunk, &chunk_heads[index], list) {
		kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", chunk->addr, chunk->size);
	}
	sb_set_color(sb_current, oldcolor);
	return 0;
}
