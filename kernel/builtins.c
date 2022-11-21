// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* builtins.c
 *
 * Handlers for each shell commands.
 *
 * created: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/11/17 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/keyboard.h>
#include <kernel/port.h>
#include <kernel/print.h>
#include <kernel/ps2.h>
#include <kernel/vga.h>

/* Reboot kernel through the PS/2 controller by sending
 * a reset signal to the CPU as soon as the ps/2 data
 * buffer is empty.
 */
void reboot()
{
	while (port_read(PS2_STATUS_REGISTER_PORT) & PS2_INPUT_BUFFER_FULL);
	port_write(PS2_STATUS_COMMAND_PORT, CPU_RESET);
	asm volatile ("hlt");
}

/* Switch to keyboard to US104 layout.
 */
void key_us()
{
	KBD_setkeymap(US104_getkeymapentry);
}

/* Switch to keyboard to FR layout.
 */
void key_fr()
{
	KBD_setkeymap(FR_getkeymapentry);
}

/* Set foreground color to red.
 */
void color_red()
{
	VGA_setforegroundcolor(VGA_COLOR_RED);
}

/* Set foreground color to default one.
 */
void color_reset()
{
	VGA_setforegroundcolor(VGA_DFL_COLOR);
}

/* Generate a divide interrupt.
 */
void int_divide()
{
	asm volatile ("int $0x0");
}

/* Clear screen.
 */
void clear()
{
	VGA_clear();
}

/* Prints usage and help message.
 */
void help()
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
    clear - clear the screen\n\
    help - print this help menu\n");
}

