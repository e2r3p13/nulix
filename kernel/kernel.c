// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
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
#include <kernel/symbole.h>
#include <kernel/paging.h>
#include <kernel/kernel.h>
#include <kernel/panic.h>
#include <kernel/isr.h>

#define NBSCREENBUF 1

struct screenbuf sb[NBSCREENBUF];
struct screenbuf *sb_current;
int sb_nbscreen = NBSCREENBUF;

int kernel_init(unsigned long multiboot_info_addr) {
	for (int i = 0; i < NBSCREENBUF; i++) {
		sb_init(sb + i);
		sb_putstr(sb + i, "Welcome to nulix-2.0.1\n");
	};
	sb_current = sb;
	sb_load(sb_current);

	if (kmalloc_init() < 0)
		PANIC;

	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();

	KBD_initialize();

	multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;
	if (kpm_init((void *)mbi->mmap_addr, mbi->mem_upper - mbi->mem_lower) < 0)
		PANIC;

	physaddr_t pagedir = page_directory_kernel_new();
	if (!pagedir)
		PANIC;

	__asm__ volatile ("movl %0, %%cr3" :: "r" (pagedir));
	__asm__ volatile ("movl %0, %%esp" :: "r" (STACK_TOP));

	kernel_main();
	return 0;
}

void kernel_main() {

	nsh();
}
