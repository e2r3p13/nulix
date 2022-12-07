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
#include <kernel/kpm.h>

#include <kernel/screenbuf.h>

extern struct screenbuf *sb;
extern int sb_index;

kpm_chunk_t g_chunk;

uint32_t pseudorand(uint32_t min, uint32_t max) {
	static uint32_t m_seed = 42;

    // Constants for the LCG
    const uint32_t a = 1103515245;
    const uint32_t c = 12345;
    const uint32_t m = (uint32_t)(1 << 16) + 1;

    // Generate the next pseudo-random number
    m_seed = (a * m_seed + c) % m;

    // Scale the random number to the desired range and return it
    return (m_seed % (max - min + 1)) + min;
}

/* Choose a pseudo random size, print it,
 * allocates a chunk of this size with kpm_alloc
 * and print the resulting chunk.
 */
void alloc() {
	size_t size;
	int res;

	size = pseudorand(1, 1 << 16);
	sb_set_fg(sb + sb_index, VGA_COLOR_GREEN);
	kprintf("Asked for allocation of %u bytes\n", size);
	res = kpm_alloc(&g_chunk, size);
	kprintf("status code: %d\n", res);
	sb_set_fg(sb + sb_index, VGA_DFL_COLOR);
	kprintf("chunk: {\n    addr = %p\n    size = %u\n}\n", g_chunk.addr, g_chunk.size);
}

void free() {
	kpm_free(&g_chunk);
}

/* Reboot kernel through the PS/2 controller by sending
 * a reset signal to the CPU as soon as the ps/2 data
 * buffer is empty.
 */
void reboot(char **argv)
{
	while (port_read_u8(PS2_STATUS_REGISTER_PORT) & PS2_INPUT_BUFFER_FULL);
	port_write_u8(PS2_STATUS_COMMAND_PORT, CPU_RESET);
	asm volatile ("hlt");
}

/* Poweroff the kernel through the qemu debug port
 */
void poweroff(char **argv)
{
	port_write_u16(0x604, 0x2000);
	asm volatile ("hlt");
}

/* Switch to keyboard to US104 layout.
 */
void key_us(char **argv)
{
	KBD_setkeymap(US104_getkeymapentry);
}

/* Switch to keyboard to FR layout.
 */
void key_fr(char **argv)
{
	KBD_setkeymap(FR_getkeymapentry);
}

/* Set foreground color to red.
 */
void color_red(char **argv)
{
	sb_set_fg(sb + sb_index, VGA_COLOR_RED);
}

/* Set foreground color to default one.
 */
void color_reset(char **argv)
{
	sb_set_color(sb + sb_index, VGA_DFL_COLOR);
}

/* Generate a divide interrupt.
 */
void int_divide(char **argv)
{
	asm volatile ("int $0x0");
}

/* Clear screen.
 */
void clear(char **argv)
{
	sb_clear(sb + sb_index);
}

/* Prints usage and help message.
 */
void help(char **argv)
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

