// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/utils.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>

extern struct screenbuf sb[];
extern int sb_index;
extern int sb_nbscreen;

extern struct builtin builtin[];

/*
 * Clear screen.
 */
int clear(int argc, char **argv)
{
	sb_clear(sb + sb_index);
	return 0;
}

/*
 * Next screen.
 */
int next(int argc, char **argv)
{
	if (sb_index + 1 < sb_nbscreen) {
		sb_unload(sb + sb_index);
		sb_index++;
		sb_load(sb + sb_index);
		return 0;
	}
	return -1;
}

/*
 * Next screen.
 */
int prev(int argc, char **argv)
{
	if (sb_index > 0) {
		sb_unload(sb + sb_index);
		sb_index--;
		sb_load(sb + sb_index);
		return 0;
	}
	return -1;
}

/*
 * Prints usage and help message.
 */
int help(int argc, char **argv)
{
	kprintf("KFS - available commands :\n");
	for (int i = 0; builtin[i].name != NULL ; i++)
		kprintf("-> %s\n", builtin[i].name);
	return 0;
}
