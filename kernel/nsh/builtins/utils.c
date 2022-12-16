// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/utils.c
 *
 * Utilitary builtins (clear/next/prev/help) file
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>
#include <kernel/stdlib.h>

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;
extern int sb_nbscreen;

extern struct builtin builtin[];

/*
 * Clear screen.
 */
int clear(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	sb_clear(sb_current);
	return 0;
}

/*
 * Next screen.
 */
int next(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	sb_unload(sb_current++);
	if ((sb_current - sb) == sb_nbscreen)
		sb_current = sb;
	sb_load(sb_current);
	return 0;
}

/*
 * Prev screen.
 */
int prev(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	sb_unload(sb_current--);
	if (sb_current < sb)
		sb_current = sb + (sb_nbscreen - 1);
	sb_load(sb_current);
	return 0;
}

/*
 * Prints usage and help message.
 */
int help(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char **argv) {
	kprintf("KFS - available commands:\n");
	for (int i = 0; builtin[i].name != NULL ; i++)
		kprintf("-> %s: %s\n", builtin[i].name, builtin[i].description);
	return 0;
}
