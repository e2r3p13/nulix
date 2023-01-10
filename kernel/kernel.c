// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyboard.h>
#include <kernel/pic_8259.h>
#include <kernel/multiboot.h>
#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/nsh.h>
#include <kernel/print.h>
#include <kernel/kmalloc.h>

#include <kernel/panic.h>

#define NBSCREENBUF 2

struct screenbuf sb[NBSCREENBUF];
struct screenbuf *sb_current;
int sb_nbscreen = NBSCREENBUF;

static int kernel_init(unsigned long multiboot_info_addr) {
	for (int i = 0; i < NBSCREENBUF; i++) {
		sb_init(sb + i);
		sb_putstr(sb + i, "Welcome to nulix-2.0.1\n");
	};
	sb_current = sb;
	sb_load(sb_current);

	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();

	if (kmalloc_init() < 0) {
		return -1;
	}

	KBD_initialize();

	multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;
	if (kpm_init((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower)) {
		return -1;
	}

	return 0;
}

void kernel_main(unsigned long multiboot_info_addr) {
	if (kernel_init(multiboot_info_addr) < 0)
		return;

	nsh();
}
