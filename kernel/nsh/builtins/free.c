// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/free.c
 *
 * Free builtin file
 *
 * created: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/stdlib.h>

#define BLTNAME "free"

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

static inline void usage() {
	kprintf("Usage: " BLTNAME " size\n");
}

/*
 * Free a memory area from addr on a given size in bytes.
 */
int free(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	uint32_t addr = strtol(argv[1], &ptr, 0);
	if (*ptr != 0) {
		kprintf(BLTNAME ": Address not well formatted.\n");
		return -1;
	}
	size_t size = strtol(argv[2], &ptr, 0);
	if (*ptr != 0) {
		kprintf(BLTNAME ": Size not well formatted.\n");
		return -1;
	}
	kpm_chunk_t chunk;
	chunk.addr = (void*)addr;
	chunk.size = size;
	kpm_free(&chunk);
	return 0;
}
