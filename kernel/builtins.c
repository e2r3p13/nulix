// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* builtins.c
 *
 * Handlers for each shell commands.
 *
 * created: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/keyboard.h>
#include <kernel/port.h>
#include <kernel/print.h>
#include <kernel/ps2.h>
#include <kernel/vga.h>

#include <kernel/screenbuf.h>
#include <kernel/string.h>

extern struct screenbuf sb[];
extern int sb_index;

/* Generate a divide interrupt.
 */
int int_divide(int argc, char **argv)
{
	asm volatile ("int $0x0");
}

/* Clear screen.
 */
int clear(int argc, char **argv)
{
	sb_clear(sb + sb_index);
}

/* Prints usage and help message.
 */
int help(int argc, char **argv)
{
	kprintf("KFS - available commands :\n\
    key-us - switch to US104 layout\n\
    key-fr - switch to FR layout\n\
    print-gdt - print all GDT informaton\n\
    print-stack - print all the stack memory\n\
    color-red - set color red to foreground\n\
    color-reset - set default color to foreground\n\
    int-divide - generate a divide error interrupt\n\
    reboot - reboot system\n\
    poweroff - poweroff system\n\
    clear - clear the screen\n\
    help - print this help menu\n");
}
