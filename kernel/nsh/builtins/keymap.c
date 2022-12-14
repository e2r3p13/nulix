// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/keymap.c
 *
 * Insert file description here
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/14 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/keyboard.h>
#include <kernel/print.h>
#include <kernel/string.h>

#define BLTNAME "keymap"

struct keymap_layout {
	char *name;
	struct kbd_keymap_entry (*keymapentry)(enum kbd_keycode);
};

static inline void usage() {
	kprintf("Usage: " BLTNAME " LAYOUT\n");
	kprintf("       " BLTNAME " list\n");
}

/*
 * Switch of keyboard layout.
 */
int keymap(int argc, char **argv) {
	struct keymap_layout layout[] = {
		{"us", US104_getkeymapentry},
		{"fr", FR_getkeymapentry},
		{NULL, NULL},
	};
	if (argc < 2) {
		usage();
		return -1;
	}
	if (!strcmp(argv[1], "list")) {
		kprintf("Layout:\n");
		for (struct keymap_layout *ptr = layout; ptr->name != NULL; ptr++) {
			kprintf("- %s\n", ptr->name);
		}
	}
	else {
		for (struct keymap_layout *ptr = layout; ptr->name != NULL; ptr++) {
			if (!strcmp(argv[1], ptr->name)) {
				KBD_setkeymap(ptr->keymapentry);
				return 0;
			}
		}
		kprintf(BLTNAME ": Layout '%s' not found.\n", argv[1]);
	}
	return -1;
}
