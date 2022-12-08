// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/keymap.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/keyboard.h>
#include <kernel/print.h>
#include <kernel/string.h>

/*
 * Switch of keyboard layout.
 */
int keymap(int argc, char **argv) {
	if (argc < 2) {
		kprintf("keymap" ": Not enough arguments.\n");
		return -1;
	}
	if (!strcmp(argv[1], "us")) {
		KBD_setkeymap(US104_getkeymapentry);
	} else if (!strcmp(argv[1], "fr")) {
		KBD_setkeymap(FR_getkeymapentry);
	} else {
		kprintf("keymap" ": Keymap '%s' not found.\n", argv[1]);
		return -1;
	}
	return 0;
}
