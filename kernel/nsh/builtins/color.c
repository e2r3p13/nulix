// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/color.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/string.h>

extern struct screenbuf sb[];
extern int sb_index;

/*
 * Change the screenbuf color
 *
 * Usage: color fg/bg COLOR 
 *        color reset
 */
int color(int argc, char **argv)
{
	char *colors[16] = {
		"black",
		"blue",
		"green",
		"cyan",
		"red",
		"magenta",
		"brown",
		"lightgrey",
		"darkgrey",
		"lightblue",
		"lightgreen",
		"lightcyan",
		"lightred",
		"lightmagenta",
		"lightbrown",
		"white"
	};
	if (argc < 2) {
		kprintf("color_change" ": Not enough arguments.\n");
		return -1;
	}
	if (!strcmp(argv[1], "reset")) {
		sb_set_color(sb + sb_index, VGA_DFL_COLOR);
	} else {
		if (argc < 3) {
			kprintf("color_change" ": Not enough arguments.\n");
			return -1;
		}
		if (!strcmp(argv[1], "fg")) {
			for (int i = 0; i < 16; i++) {
				if (!strcmp(colors[i], argv[2]))
					sb_set_fg(sb + sb_index, i);
			}
		} else if (!strcmp(argv[1], "bg")) {
			for (int i = 0; i < 16; i++) {
				if (!strcmp(colors[i], argv[2]))
					sb_set_bg(sb + sb_index, i);
			}
		} else {
			kprintf("color_change" ": Unknown command.\n");
			return -1;
		}
	}
	return 0;
}
