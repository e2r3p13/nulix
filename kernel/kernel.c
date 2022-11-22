// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/22 - mrxx0 <lfalkau@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyboard.h>
#include <kernel/pic_8259.h>
#include <kernel/vga.h>
#include <kernel/builtins.h>
#include <kernel/print.h>
#include <kernel/multiboot.h>

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
	kprintf("0x%x\n", (unsigned)mbi->flags);
	kprintf("mem_lower : %p\n", (unsigned)mbi->mem_lower);
	kprintf("mem_upper : %p\n", (unsigned)mbi->mem_upper);

	shell();
}
