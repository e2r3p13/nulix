// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/color.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/screenbuf.h>
#include <kernel/print.h>
#include <kernel/string.h>

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

#define BLTNAME "color"

static inline void usage() {
	kprintf("Usage: " BLTNAME " fg/bg COLOR\n");
	kprintf("       " BLTNAME " reset\n");
}

/*
 * Change the screenbuf color
 *
 * Usage: color fg/bg COLOR 
 *        color reset
 */
int color(int argc, char **argv) {
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
		usage();
		return -1;
	}
	if (!strcmp(argv[1], "reset")) {
		sb_set_color(sb_current, SB_DFL_COLOR);
	} else {
		if (argc < 3) {
			usage();
			return -1;
		}
		if (!strcmp(argv[1], "fg")) {
			for (int i = 0; i < 16; i++) {
				if (!strcmp(colors[i], argv[2])) {
					sb_set_fg(sb_current, i);
					return 0;
				}
			}
			kprintf(BLTNAME ": '%s' unknown color.\n", argv[2]);
		} else if (!strcmp(argv[1], "bg")) {
			for (int i = 0; i < 16; i++) {
				if (!strcmp(colors[i], argv[2])) {
					sb_set_bg(sb_current, i);
					return 0;
				}
			}
			kprintf(BLTNAME ": '%s' unknown color.\n", argv[2]);
		} else {
			kprintf(BLTNAME ": '%s' unknown command.\n", argv[1]);
		}
	}
	return -1;
}
