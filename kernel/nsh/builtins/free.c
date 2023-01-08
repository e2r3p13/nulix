// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/free.c
 *
 * Free builtin file
 *
 * created: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/stdlib.h>

#define BLTNAME "free"

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

#define MAXALLOC 10
extern struct kpm_chunk_head chunk_heads[];
extern uint8_t alloc_bitmap_array[];
extern struct bitmap alloc_bitmap;
extern int alloc_bitmap_init;

static inline void usage() {
	kprintf("Usage: " BLTNAME " size\n");
}

/*
 * Free a memory area from addr on a given size in bytes.
 */
int free(int argc, char **argv) {
	if (!alloc_bitmap_init) {
		alloc_bitmap_init = 0;
		bitmap_init(&alloc_bitmap, MAXALLOC, alloc_bitmap_array, ((MAXALLOC - 1) / 8) + 1);
	}

	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	uint32_t to_free = strtoul(argv[1], &ptr, 0);
	if (*ptr != 0 || argv[1][0] == '-') {
		kprintf(BLTNAME ": Index not well formatted.\n");
		return -1;
	}
	if (to_free >= MAXALLOC) {
		kprintf(BLTNAME ": Index too big.\n");
		return -1;
	}
	if (bitmap_get_at(&alloc_bitmap, to_free) > 0) {
		kpm_free(&chunk_heads[to_free]);
		bitmap_set_at(&alloc_bitmap, to_free, 0);
	}
	return 0;
}
