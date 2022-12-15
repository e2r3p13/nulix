// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/kexdump.c
 *
 * Insert file description here
 *
 * created: 2022/12/09 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/12 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/stdlib.h>
#include <kernel/screenbuf.h>
#include <kernel/print.h>

#define BLTNAME "hexdump"

#define NBYTES_LINE	16
#define C_ACTIVE	SB_COLOR_LIGHT_RED
#define C_INACTIVE	SB_DFL_FG_COLOR
#define C_NONPRINT	SB_DFL_FG_COLOR

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

static inline void usage() {
	kprintf("Usage: " BLTNAME " address size\n");
}

static void hx_printline(void *base, size_t nbytes) {
	uint8_t byte;
	kprintf("%8p: ", base);
	for (int i = 0, nb = (int)nbytes; i < NBYTES_LINE; i++, nb--) {
		if (i % 2 == 0)
			kprintf(" ");
		if (nb > 0) {
			byte = *(uint8_t *)(base + i);
			sb_set_fg(sb_current, byte ? C_ACTIVE : C_INACTIVE);
			kprintf("%2x", byte);
			sb_set_fg(sb_current, C_INACTIVE);
		} else {
			kprintf("  ");
		}
	}
	sb_set_fg(sb_current, C_INACTIVE);
	kprintf("  |");
	for (int i = 0, nb = (int)nbytes; i < NBYTES_LINE; i++, nb--) {
		if (nb > 0) {
			byte = *(uint8_t *)(base + i);
			if (byte == 0) {
				sb_set_fg(sb_current, C_INACTIVE);
				byte = '.';
			} else if (byte < 32 || byte > 127) {
				sb_set_fg(sb_current, C_NONPRINT);
				byte = '.';
			} else {
				sb_set_fg(sb_current, C_ACTIVE);
			}
			kprintf("%c", byte);
			sb_set_fg(sb_current, C_INACTIVE);
		} else {
			kprintf(" ");
		}
	}
	kprintf("|\n");
}

void hx_print(void *base, size_t size) {
	enum sb_color color = sb_get_color(sb_current);
	sb_set_color(sb_current, SB_DFL_COLOR);
	sb_set_fg(sb_current, C_INACTIVE);
	for (;size >= NBYTES_LINE; size -= NBYTES_LINE) {
		hx_printline(base, NBYTES_LINE);
		base += NBYTES_LINE;
	}
	if (size)
		hx_printline(base, size);
	sb_set_color(sb_current, color);
}

int hexdump(int argc, char **argv) {
	if (argc < 3) {
		usage();
		return -1;
	}
	char *ptr;
	uint32_t addr = strtol(argv[1], &ptr, 0);
	if (*ptr != 0) {
		kprintf(BLTNAME ": Address not well formated.\n");
		return -1;
	}

	size_t size = strtol(argv[2], &ptr, 0);
	if (*ptr != 0) {
		kprintf(BLTNAME ": Size not well formated.\n");
		return -1;
	}

	hx_print((void *)addr, size);

	return 0;
}
