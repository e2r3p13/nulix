// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
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
int sb_index = 0;
int sb_nbscreen = NBSCREENBUF;

/* Initialize all descriptor tables (gdt, idt, ...)
 *
 */
static void init_descriptor_tables() {
	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();
}

void kernel_main(unsigned long multiboot_info_addr) {
	multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;

	init_descriptor_tables();
	KBD_initialize();

	kpm_init((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower);

	for (int i = 0; i < NBSCREENBUF; i++) {
		sb_init(sb + i);
		sb_putstr(sb + i, "Welcome to nulix-2.0.1\n");
	};
	sb_load(sb + sb_index);

	nsh();
}
