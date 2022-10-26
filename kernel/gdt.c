// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.c
 *
 * Fill the gdt entries and load it.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/11/21 - lfalkau <lfalkau@student.42.fr>
 */

#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/keyboard.h>
#include <kernel/vga.h>
#include <kernel/print.h>

#include "gdt_internal.h"

extern void gdt_flush();

/* Pointer to the gdt. */
static t_gdt_entry *gdt = (t_gdt_entry *)GDT_BASE;

/* Store a pointer to the gdt and its size. */
static t_gdt_ptr gdtp;

/* Fill an given entry to the gdt.
 *
 */
static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

/* Initialize the gdt with NULL entry, Kernel Code entry, Kernel Data entry,
 * User Code entry and User Data entry.
 */
void gdt_init()
{
	gdtp.limit = (sizeof(t_gdt_entry) * 6);
	gdtp.base = (uint32_t)gdt;
	gdt_set_gate(0,	0,	0x0000,	0,	0);	// NULL descriptor
	gdt_set_gate(1,	0,	0xFFFF,	0x9A,	0xCF);	// Kernel Mode Code Segment
	gdt_set_gate(2,	0,	0xFFFF,	0x92,	0xCF);	// Kernel Mode Data Segment
	gdt_set_gate(3,	0,	0xFFFF,	0x96,	0xCF);	// Kernel Mode Stack Segment
	gdt_set_gate(4,	0,	0xFFFF,	0xFA,	0xCF);	// User Mode Code Segment
	gdt_set_gate(5,	0,	0xFFFF,	0xF2,	0xCF);	// User Mode Data Segment
	gdt_set_gate(6,	0,	0xFFFF,	0xF6,	0xCF);	// User Mode Stack Segment
	gdt_flush(&gdtp);
}

/* Print all the info from the gdt basic info and all of the entries
 *
 */
void gdt_print() {
	VGA_setforegroundcolor(VGA_COLOR_LIGHT_BROWN);
	kprintf("-- GDT --\n");
	VGA_setforegroundcolor(VGA_COLOR_WHITE);
	kprintf("Base: %8p\n", gdt);
	kprintf("Limit: %p\n", gdtp.limit);
	kprintf("Entries:\n\n");
	VGA_setforegroundcolor(VGA_COLOR_LIGHT_BROWN);
	kprintf("BASE LOW    BASE MID    BASE HIGH   LIMIT LOW   GRAN        ACC\n");
	VGA_setforegroundcolor(VGA_COLOR_WHITE);
	for (uint8_t i = 0; i < GDT_SIZE; i++) {
		kprintf("%4p      ", gdt[i].base_low);
		kprintf("%4p      ", gdt[i].base_middle);
		kprintf("%4p      ", gdt[i].base_high);
		kprintf("%4p      ", gdt[i].limit_low);
		kprintf("%4p      ", gdt[i].granularity);
		kprintf("%4p      ", gdt[i].access);
		kprintf("\n");
	}
}
