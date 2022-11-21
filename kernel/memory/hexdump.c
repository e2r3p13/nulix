// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* hexdump.c
 *
 * Exports a hexdump function to print memory
 *
 * created: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/21 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/vga.h>
#include <kernel/print.h>
#include <kernel/keyboard.h>

extern struct vga vga;

static void wait_for_char_input(int c) {
	struct kbd_event evt;

	while (1) {
		while (!KBD_poll());
		KBD_getevent(&evt);
		if (evt.type == KEY_PRESSED) {
			if (c == 0 || KBD_getchar(&evt) == c)
				break;
		}
	}
}

static int hexdump_diaplay_char(int c) {
	if (c == 0) {
		VGA_setforegroundcolor(VGA_COLOR_WHITE);
		return '.';
	} else if (c < 32 || c > 127) {
		VGA_setforegroundcolor(VGA_COLOR_LIGHT_BLUE);
		return '.';
	} else {
		VGA_setforegroundcolor(VGA_COLOR_LIGHT_GREEN);
		return c;
	}
}

/* Hexdump to print from address to limit
 *
 * @base is the address to start the print from
 * @limit is number of bytes we want to print
 *
 */
void hexdump(void *base, uint32_t limit) {
	struct kbd_event evt;
	uint32_t l = 0, i = 0;
	uint8_t oldcolor = vga.color;
	uint8_t *p = (uint8_t *)base;

	VGA_setforegroundcolor(VGA_COLOR_WHITE);
	while (i < limit) {
		kprintf("%8p:  ", p + i);
		for (uint32_t j = i; j < limit && j < i + 16; j++) {
			VGA_setforegroundcolor(p[j] ? VGA_COLOR_LIGHT_GREEN : VGA_COLOR_WHITE);
			kprintf("%2x ", p[j]);
		}
		kprintf(" ");
		for (uint32_t j = i; j < limit && j < i + 16; j++) {
			kprintf("%c", hexdump_diaplay_char(p[j]));
		}
		l += 1;
		if (l == VGA_HEIGHT) {
			wait_for_char_input(' ');
			VGA_clear();
			l = 0;
		} else {
			kprintf("\n");
		}
		VGA_setforegroundcolor(VGA_COLOR_WHITE);
		i += 16;
	}
	vga.color = oldcolor;
}

