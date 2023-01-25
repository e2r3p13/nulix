// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/interrupt.c
 *
 * Interrupt builtin file
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
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
		case 1:
			__asm__ volatile ("int $1");
			break;
		case 2:
			__asm__ volatile ("int $2");
			break;
		case 3:
			__asm__ volatile ("int $3");
			break;
		case 4:
			__asm__ volatile ("int $4");
			break;
		case 5:
			__asm__ volatile ("int $5");
			break;
		case 6:
			__asm__ volatile ("int $6");
			break;
		case 7:
			__asm__ volatile ("int $7");
			break;
		case 9:
			__asm__ volatile ("int $9");
			break;
		case 16:
			__asm__ volatile ("int $16");
			break;
		case 18:
			__asm__ volatile ("int $18");
			break;
		case 19:
			__asm__ volatile ("int $19");
			break;
		case 20:
			__asm__ volatile ("int $20");
			break;
		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 17:
			kprintf(BLTNAME ": interrupt number '%d' can't be raised.\n", interrupt);
			break;
		default:
			kprintf(BLTNAME ": interrupt number '%d' not implemented.\n", interrupt);
			break;
	}
	return 0;
}
