// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/25 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyboard.h>
#include <kernel/pic_8259.h>
#include <kernel/vga.h>
#include <kernel/builtins.h>
#include <kernel/print.h>
#include <kernel/multiboot.h>
#include <kernel/kpm.h>

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
	VGA_initialize();
	KBD_initialize();
	kpm_init((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower);

	shell();
}
