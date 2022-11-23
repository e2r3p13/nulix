// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* shell.c
 *
 * Managing shell.
 *
 * created: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/11/23 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/builtins.h>
#include <kernel/gdt.h>
#include <kernel/keyboard.h>
#include <kernel/memory.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/vga.h>
#include <kernel/kpm.h>

#define HASH_VALUE 5381
#define HASH_MOD 100
#define BUFSIZE 32

/* Static buffer to hold the command.
 */
static char buffer[BUFSIZE];

/* Array of functions to the shell command.
 */
static void (*f_ptr[HASH_MOD])() = {
	[14] = reboot,
	[20] = gdt_print,
	[21] = key_us,
	[29] = print_stack,
	[30] = color_red,
	[37] = int_divide,
	[52] = clear,	
	[60] = help,
	[62] = poweroff,
	[88] = buddy_print,
	[90] = key_fr,
	[98] = color_reset,
};

/* Print basic prompt to emulate a shell.
 */
static void print_prompt()
{
	kprintf("$>");
}

/* Handle unknown command.
 */
static void error_cmd()
{
	kprintf("ksh: %s: command not found\n", buffer);
}

/* Basic hash function to return a unique hash value for each command.
 * 
 * @to_hash : the string to hash
 */
static uint32_t dj2b_hash(char *to_hash)
{
	uint32_t hash;
	uint32_t c;

	hash = HASH_VALUE;
	while ((c = *to_hash++))
		hash = c + (hash << 1);
	return hash % HASH_MOD;
}

/* Handling command by executing it if it exists, otherwise handle the error.
 */
static void handle_command()
{
	uint32_t hash = dj2b_hash(buffer);
	
	if (hash > HASH_MOD || f_ptr[hash] == 0)
		error_cmd();
	else
		f_ptr[hash]();
}

/* Filling our command buffer with user input.
 */
static void fill_buffer(char c, uint8_t i)
{
	if (i < BUFSIZE - 1)
		buffer[i] = c;
}

/* Basic POSIX like shell to execute some commands.
 */
void shell()
{
	struct kbd_event evt;
	char c = 0;
	uint8_t i = 0;

	print_prompt();
	memset(buffer, '\0', BUFSIZE);
	while (1) {
		while (!KBD_poll());
		if (KBD_getevent(&evt) == 0 && evt.type == KEY_PRESSED) {
			if ((c = KBD_getchar(&evt))) {
				if (c == '\n') {
					VGA_putchar(c);
					if (i > 0)
						handle_command();
					print_prompt();
					memset(buffer, '\0', BUFSIZE);
					i = 0;
				}
				else if (c == '\b') {
					if (i > 0) {
						i--;
						VGA_putchar(c);
						buffer[i] = '\0';
					}	
				}
				else if (i < BUFSIZE - 1) {
					fill_buffer(c, i);
					VGA_putchar(c);
					i++;
				}
			}
		}
	}
}
