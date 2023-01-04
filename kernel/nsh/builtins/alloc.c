// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/alloc.c
 *
 * Allocate builtin file
 *
 * created: 2022/12/13 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/04 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/stdlib.h>
#include <kernel/list.h>

#define BLTNAME "alloc"

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

static inline void usage() {
	kprintf("Usage: " BLTNAME " size\n");
}

const int MAXALLOC = 10;
struct kpm_chunk_head chunk_heads[MAXALLOC] = {0};
int nalloc = 0;

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
	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	int32_t size = strtol(argv[1], &ptr, 0);
	if (*ptr != 0 || size < 0) {
		kprintf(BLTNAME ": Size not well formatted.\n");
		return -1;
	}
	if (nalloc == MAXALLOC) {
		kprintf(BLTNAME ": Max allocation reached (%d).\n", MAXALLOC);
		return -1;
	}
	kprintf("Asked for allocation of %d bytes\n", size);
	struct kpm_chunk *chunk;
	if (kpm_alloc(&chunk_heads[nalloc], size) < 0) {
		kprintf("Failed to allocate memory area.\n");
		return -1;
	}
	kprintf("Allocation %d/%d\n", nalloc, MAXALLOC);

	uint8_t oldcolor = sb_get_color(sb_current);
	sb_set_fg(sb_current, SB_COLOR_GREEN);
	TAILQ_FOREACH(chunk, &chunk_heads[nalloc], list) {
		kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", chunk->addr, chunk->size);
	}
	nalloc++;
	sb_set_color(sb_current, oldcolor);
	return 0;
}
