// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/15 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyboard.h>
#include <kernel/pic_8259.h>
#include <kernel/multiboot.h>
#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>

#define NBSCREENBUF 2

struct screenbuf sb[NBSCREENBUF];
struct screenbuf *sb_current;
int sb_nbscreen = NBSCREENBUF;

extern buddy_t *buddy;

/* Initialize all descriptor tables (gdt, idt, ...)
 *
 */
static void init_descriptor_tables() {
	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();
}

void kernel_init_kpm(unsigned long multiboot_info_addr) {
	char cset[] = "0123456789abcdef";
	uint16_t *vga = (uint16_t*)0xb8000;

	*vga = 0x0f00 | 0x39;
	vga++;
	multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;
	(&kpm_init - 0xc0000000)((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower);

	uint32_t ptr = (uint32_t)(buddy - 0xc0000000);

	*vga = 0x0f00 | cset[(ptr >> 28) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 24) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 20) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 16) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 12) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 8) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr >> 4) & 0x0f];
	vga++;
	*vga = 0x0f00 | cset[(ptr) & 0x0f];
	vga++;
}

void kernel_main() {

	init_descriptor_tables();
	KBD_initialize();

	kpm_init((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower);

	for (int i = 0; i < NBSCREENBUF; i++) {
		sb_init(sb + i);
		sb_putstr(sb + i, "Welcome to nulix-2.0.1\n");
	};
	sb_current = sb;
	sb_load(sb_current);

	nsh();
}
