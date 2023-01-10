// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/11 - xlmod <glafond-@student.42.fr>
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

#define NBSCREENBUF 1

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

	kprintf("SYM_KERNEL_VIRT_START: %p\n", SYM_KERNEL_VIRT_START);
	kprintf("SYM_KERNEL_END:        %p\n", SYM_KERNEL_END);
	kprintf("SYM_TEXT:         %8p\n", SYM_TEXT);
	kprintf("SYM_VIRT_TEXT:    %8p\n", SYM_VIRT_TEXT);
	kprintf("SYM_RODATA:       %8p\n", SYM_RODATA);
	kprintf("SYM_VIRT_RODATA:  %8p\n", SYM_VIRT_RODATA);
	kprintf("SYM_DATA:         %8p\n", SYM_DATA);
	kprintf("SYM_VIRT_DATA:    %8p\n", SYM_VIRT_DATA);
	kprintf("SYM_BSS:          %8p\n", SYM_BSS);
	kprintf("SYM_VIRT_BSS:     %8p\n", SYM_VIRT_BSS);
	kprintf("SYM_BUDDY:        %8p\n", SYM_BUDDY);
	kprintf("SYM_VIRT_BUDDY:   %8p\n", SYM_VIRT_BUDDY);
	kprintf("SYM_STACK:        %8p\n", SYM_STACK);
	kprintf("SYM_VIRT_STACK:   %8p\n", SYM_VIRT_STACK);
	kprintf("SYM_HEAP:         %8p\n", SYM_HEAP);
	kprintf("SYM_VIRT_HEAP:    %8p\n", SYM_VIRT_HEAP);
	kprintf("STACK_TOP:        %8p\n", STACK_TOP);
	kprintf("STACK_BOTTOM:     %8p\n", STACK_BOTTOM);

	if (kmalloc_init() < 0) {
		return -1;
	}

	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();

	KBD_initialize();

	multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;
	if (kpm_init((void *)mbi->mmap_addr,
		mbi->mmap_length / sizeof(struct multiboot_mmap_entry),
		mbi->mem_upper - mbi->mem_lower)) {
		return -1;
	}

	// Remove Identity Mapping
	__asm__ volatile ("movl $0x1000, %eax\n"
	"movl $0, (%eax)\n"
	"movl %cr3, %eax\n"
	"movl %eax, %cr3\n");

	return 0;
}

void kernel_main(unsigned long multiboot_info_addr) {
	if (kernel_init(multiboot_info_addr) < 0)
		return;

	nsh();
}
