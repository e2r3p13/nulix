// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/alloc.c
 *
 * Allocate builtins file
 *
 * created: 2022/12/13 - glafond- <glafond-@student.42.fr>
 * updated: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/stdlib.h>

#define BLTNAME "alloc"

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

static inline void usage() {
	kprintf("Usage: " BLTNAME " size\n");
}

/* Wrapper for the kpm_alloc function.
 * Loop until a maximum number of chunk has been allocated to allocate the 
 * given memory size.
 *
 * */

int alloc_loop(kpm_chunk_t *chunk, size_t size) {
	uint8_t oldcolor = sb_get_color(sb_current);
	sb_set_fg(sb_current, SB_COLOR_GREEN);
	while (size > 0) {
		if (kpm_alloc(chunk, size) < 0)
			return -1;
		kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", chunk->addr, chunk->size);
		if (chunk->size >= size)
			break ;
		size -= chunk->size;
	}
	sb_set_fg(sb_current, oldcolor);
	return 0;
}

/* Choose a pseudo random size, print it,
 * allocates a chunk of this size with kpm_alloc
 * and print the resulting chunk.
 */
int alloc(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	int size = strtol(argv[1], &ptr, 0);
	if (*ptr != 0) {
		kprintf(BLTNAME ": Size not well formatted.\n");
		return -1;
	}
	kpm_chunk_t chunk;
	kprintf("Asked for allocation of %d bytes\n", size);
	if (alloc_loop(&chunk, size) == -1)
		kprintf("Failed to allocate memory area.\n");
	return 0;
}
