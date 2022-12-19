// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/nsh.c
 *
 * Nulix shell
 *
 * created: 2022/12/07 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/string.h>
#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>
#include <kernel/keyboard.h>
#include <kernel/builtins.h>

extern struct screenbuf sb[];
extern struct screenbuf *sb_current;

// Buffer where the user input is stored
char nsh_buf[NSH_BUFSIZE];
char nsh_old[NSH_BUFSIZE];
// Buffer where the splitted cmd is stored
char *nsh_cmd[NSH_BUFSIZE / 2 + 1];

int nsh_cmdnarg;
int nsh_bufindex;

/*
 * Array of struct builtin with a function pointer to each builtin
 */
struct builtin builtin[] = {
	{"reboot", reboot, "Reboot the machine"},
	{"poweroff", poweroff, "Power-off the machine"},
	{"exit", poweroff, "Alias of 'poweroff'"},
	{"keymap", keymap, "Switch keyboard layout"},
	{"color", color, "Change the screen buffer color"},
	{"alloc", alloc, "Allocate a memory area"},
	{"free", free, "Free a given memory area"},
	{"info", info, "Print some information about system"},
	{"clear", clear, "Clear the screen buffer"},
	{"hexdump", hexdump, "Dump a memory zone"},
	{"next", next, "Switch to next screen buffer"},
	{"prev", prev, "Switch to prev screen buffer"},
	{"help", help, "Print help"},
	{"interrupt", interrupt, "Raise an interrupt"},
	{NULL, NULL, NULL},
};

/*
 * Print a prompt for the shell
 */
static void nsh_prompt() {
	uint8_t oldcolor;

	oldcolor = sb_get_color(sb_current);
		sb_set_fg(sb_current, SB_COLOR_LIGHT_BLUE);
	kprintf("nsh$ ");
	sb_set_color(sb_current, oldcolor);
}

/*
 * Clear the buffers and print the prompt
 */
static void nsh_newline() {
	memset(nsh_buf, 0 , NSH_BUFSIZE);
	memset(nsh_cmd, 0 , NSH_BUFSIZE / 2);
	nsh_bufindex = 0;
	nsh_cmdnarg = 0;
	nsh_prompt();
}

/*
 * Split the user input
 * Get the first char of a word into the cmd buffer and set the space at the
 * end of a word to a 0 while moving in the buffer.
 */
static void nsh_createcmd() {
	char c;
	int ibuf = 0;
	int icmd = 0;

	memset(nsh_old, 0, NSH_BUFSIZE);
	memcpy(nsh_old, nsh_buf, nsh_bufindex);
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

/*
 * Iterrate through all builtins and if the first element of the cmd buffer
 * is equal to a builtin name, execute the function.
 */
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
	kprintf("nsh: %s: command not found\n", nsh_cmd[0]);
}

/*
 * Handle non character input
 */
static void nsh_shortcut(struct kbd_event *evt) {
	switch (evt->key) {
		case KEY_CURSOR_UP:
			sb_scroll(sb_current, -1);
			break;
		case KEY_CURSOR_DOWN:
			sb_scroll(sb_current, 1);
			break;
		case KEY_PAGE_DOWN:
			sb_scroll_down(sb_current);
			break;
		case KEY_PAGE_UP:
			sb_scroll_top(sb_current);
			break;
		case KEY_CURSOR_LEFT:
			for (int i = 0; i < nsh_bufindex; i++)
				sb_putchar(sb_current, '\b');
			nsh_bufindex = strlen(nsh_old);
			memcpy(nsh_buf, nsh_old, nsh_bufindex);
			sb_putstr(sb_current, nsh_buf);
			break;
		default:
			break;
	}
}

/*
 * Handle character input
 */
static void nsh_addchar(char c) {
	// put the viewport where the cursor is
	sb_scroll_down(sb_current);

	if (c == '\n') {
		// New line
		kprintf("\n");
		if (nsh_bufindex != 0) {
			nsh_createcmd();
			nsh_execcmd();
		}
		nsh_newline();
	} else if (c == '\b') {
		// Backspace
		if (nsh_bufindex > 0) {
			nsh_buf[--nsh_bufindex] = 0;
			kprintf("\b");
		}
	} else if (c == '\t') {
		// Use tabulation to do autocompletion
		if (nsh_bufindex == 0)
			return;
		for (int i = 0; builtin[i].name != NULL; i++) {
			char *p = nsh_buf;
			char *q = builtin[i].name;
			while (*p && *q && *p == *q)
				p++, q++;
			if (*p == 0 && *q != 0) {
				kprintf("%s", q);
				while (*q && nsh_bufindex < NSH_BUFSIZE)
					nsh_buf[nsh_bufindex++] = *q++;
			}
		}
	} else {
		// Other char
		if (nsh_bufindex < NSH_BUFSIZE - 1) {
			nsh_buf[nsh_bufindex++] = c;
			kprintf("%c", c);
		}
	}
}

/*
 * Main nsh function with a infinite loop 
 */
void nsh() {
	struct kbd_event evt;
	char c;

	nsh_newline();
	while (1) {
		KBD_geteventbytype(&evt, KEY_PRESSED);
		if ((c = KBD_getchar(&evt)))
			nsh_addchar(c);
		else
			nsh_shortcut(&evt);
	}
}
