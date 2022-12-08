// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/nsh.c
 *
 * Nulix shell
 *
 * created: 2022/12/07 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/08 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/string.h>
#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>
#include <kernel/keyboard.h>
#include <kernel/builtins.h>

extern struct screenbuf sb[];
extern int sb_index;

char nsh_buf[NSH_BUFSIZE];
char *nsh_cmd[NSH_BUFSIZE / 2];
int nsh_cmdnarg;
int nsh_bufindex;

struct builtin builtin[] = {
	{"reboot", reboot},
	{"poweroff", poweroff},
	{"exit", poweroff},
	{"keymap", keymap},
	{"color", color},
	{"alloc", alloc},
	{"free", free},
	{"clear", clear},
	{"next", next},
	{"prev", prev},
	{"help", help},
	{"interrupt", interrupt},
	{NULL, NULL},
};


static void nsh_prompt() {
	kprintf("$> ");
}

static void nsh_newline() {
	memset(nsh_buf, 0 , NSH_BUFSIZE);
	memset(nsh_cmd, 0 , NSH_BUFSIZE / 2);
	nsh_bufindex = 0;
	nsh_cmdnarg = 0;
	nsh_prompt();
}

static void nsh_createcmd() {
	char c;
	int ibuf = 0;
	int icmd = 0;
	while (ibuf < nsh_bufindex) {
		c = nsh_buf[ibuf];
		if (c != ' ' && c != 0) {
			nsh_cmd[icmd++] = nsh_buf + ibuf;
			while (nsh_buf[ibuf] != ' ' && nsh_buf[ibuf] != 0)
				ibuf++; 
		} else if (c == ' ') {
			nsh_buf[ibuf++] = 0;
			while (nsh_buf[ibuf] == ' ')
				ibuf++; 
		}
	}
	nsh_cmdnarg = icmd;
}

static void nsh_execcmd() {
	int n = sizeof(builtin) / sizeof(struct builtin);
	if (nsh_cmd[0] == NULL)
		return;
	for (int i = 0; i < n; i++) {
		if (strcmp(nsh_cmd[0], builtin[i].name) == 0) {
			builtin[i].exec(nsh_cmdnarg, (char **)nsh_cmd);
			return;
		}
	}
	kprintf("nsh" ": %s: command not found...\n", nsh_cmd[0]);
}

static void nsh_getkeyevent(struct kbd_event *evt) {
	while (1) {
		while (!KBD_poll());
		if (KBD_getevent(evt) == 0 && evt->type == KEY_PRESSED)
			return ;
	}
}

static void nsh_shortcut(struct kbd_event *evt) {
	switch (evt->key) {
		case KEY_CURSOR_UP:
			sb_scroll(sb + sb_index, -1);
			break;
		case KEY_CURSOR_DOWN:
			sb_scroll(sb + sb_index, 1);
			break;
		default:
			break;
	}
}

static void nsh_addchar(char c) {
	if (c == '\n') {
		kprintf("\n");
		if (nsh_bufindex != 0) {
			nsh_createcmd();
			nsh_execcmd();
		}
		nsh_newline();
	} else if (c == '\b') {
		if (nsh_bufindex > 0) {
			nsh_buf[--nsh_bufindex] = 0;
			kprintf("\b");
		}
	} else {
		if (nsh_bufindex < NSH_BUFSIZE - 1) {
			nsh_buf[nsh_bufindex++] = c;
			kprintf("%c", c);
		}
	}
}

void nsh() {
	struct kbd_event evt;
	char c;

	nsh_newline();
	while (1) {
		nsh_getkeyevent(&evt);
		if ((c = KBD_getchar(&evt)))
			nsh_addchar(c);
		else
			nsh_shortcut(&evt);
	}
}
