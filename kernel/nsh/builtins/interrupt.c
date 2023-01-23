// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/interrupt.c
 *
 * Interrupt builtin file
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/23 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/stdlib.h>
#include <kernel/print.h>

#define BLTNAME "interrupt"

static inline void usage() {
	kprintf("Usage: " BLTNAME " int\n");
}

int interrupt(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return -1;
	}
	char *ptr;
	int interrupt = strtol(argv[1], &ptr, 0);
	if (*ptr) {
		kprintf(BLTNAME ": interrupt number not well formated.\n");
		return -1;
	}
	switch (interrupt) {
		case 0:
			__asm__ volatile ("int $0");
			break;
		case 4:
			__asm__ volatile ("int $4");
			break;
		case 8:
			kprintf(BLTNAME ": interrupt number '%d' can't be raised.\n", interrupt);
			break;
		default:
			kprintf(BLTNAME ": interrupt number '%d' not implemented.\n", interrupt);
			__asm__ volatile ("int $7");
	}
	return 0;
}
