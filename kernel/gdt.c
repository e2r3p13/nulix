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

extern struct vga vga;

/* GDT entries information
 */
#define GDT_BASE_ADDRESS 0x800
#define GDT_SIZE 0x6
#define GDT_KERNEL_LIMIT 0xFFFF
#define GDT_USER_LIMIT 0xFFFF
#define GDT_GRAN_BASE 0xCF
#define GDT_ENTRY_NULL 0x0
#define GDT_KERNEL_CODE 0x1
#define GDT_KERNEL_DATA 0x2
#define GDT_KERNEL_STACK 0x3
#define GDT_USER_CODE 0x4
#define GDT_USER_DATA 0x5
#define GDT_USER_STACK 0x6
#define GDT_BASE_SEGMENT 0x0

/* GDT entries operations to retrieve segment value and segment access
 */
#define SEG_DESCTYPE(x) ((x) << 0x04)
#define SEG_PRES(x) ((x) << 0x07)
#define SEG_SAVL(x) ((x) << 0x0C)
#define SEG_LONG(x) ((x) << 0x0D)
#define SEG_SIZE(x) ((x) << 0x0E)
#define SEG_GRAN(x) ((x) << 0x0F)
#define SEG_PRIV(x) (((x)&0x03) << 0x05)
#define SEG_DATA_RD 0x00        // Read-Only
#define SEG_DATA_RDA 0x01       // Read-Only, accessed
#define SEG_DATA_RDWR 0x02      // Read/Write
#define SEG_DATA_RDWRA 0x03     // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05   // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06  // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08        // Execute-Only
#define SEG_CODE_EXA 0x09       // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A      // Execute/Read
#define SEG_CODE_EXRDA 0x0B     // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C       // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D      // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E     // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F    // Execute/Read, conforming, accessed

/* GDT_KERNEL_CODE_ACCESS - 0x9A */
#define GDT_KERNEL_CODE_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_CODE_EXRD

/* GDT_KERNEL_DATA_ACCESS - 0x92 */
#define GDT_KERNEL_DATA_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_DATA_RDWR

/* GDT_KERNEL_STACK_ACCESS - 0x96 */
#define GDT_KERNEL_STACK_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(0) | SEG_DATA_RDWREXPD

/* GDT_USER_CODE_ACCESS - 0xFA */
#define GDT_USER_CODE_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_CODE_EXRD

/* GDT_USER_DATA_ACCESS - 0xF2 */
#define GDT_USER_DATA_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_DATA_RDWR

/* GDT_USER_STACK_ACCESS - 0xF6 */
#define GDT_USER_STACK_ACCESS SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(3) | SEG_DATA_RDWREXPD

extern void gdt_flush();

/* Pointer to the gdt. */
static t_gdt_entry *gdt = (t_gdt_entry *)GDT_BASE_ADDRESS;

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
	gdtp.limit = (sizeof(t_gdt_entry) * GDT_SIZE);
	gdtp.base = (uint32_t)gdt;
	gdt_set_gate(GDT_ENTRY_NULL,	GDT_ENTRY_NULL,		GDT_ENTRY_NULL,		GDT_ENTRY_NULL,							GDT_ENTRY_NULL);	// NULL descriptor
	gdt_set_gate(GDT_KERNEL_CODE,	GDT_BASE_SEGMENT,	GDT_KERNEL_LIMIT,	(uint8_t)(GDT_KERNEL_CODE_ACCESS),		GDT_GRAN_BASE);		// Kernel Mode Code Segment
	gdt_set_gate(GDT_KERNEL_DATA,	GDT_BASE_SEGMENT,	GDT_KERNEL_LIMIT,	(uint8_t)(GDT_KERNEL_DATA_ACCESS),		GDT_GRAN_BASE);		// Kernel Mode Data Segment
	gdt_set_gate(GDT_KERNEL_STACK,	GDT_BASE_SEGMENT,	GDT_KERNEL_LIMIT,	(uint8_t)(GDT_KERNEL_STACK_ACCESS),		GDT_GRAN_BASE);		// Kernel Mode Stack Segment
	gdt_set_gate(GDT_USER_CODE,		GDT_BASE_SEGMENT,	GDT_USER_LIMIT,		(uint8_t)(GDT_USER_CODE_ACCESS),			GDT_GRAN_BASE);		// User Mode Code Segment
	gdt_set_gate(GDT_USER_DATA,		GDT_BASE_SEGMENT,	GDT_USER_LIMIT,		(uint8_t)(GDT_USER_DATA_ACCESS),			GDT_GRAN_BASE);		// User Mode Data Segment
	gdt_set_gate(GDT_USER_STACK,	GDT_BASE_SEGMENT,	GDT_USER_LIMIT,		(uint8_t)(GDT_USER_STACK_ACCESS),			GDT_GRAN_BASE);		// User Mode Stack Segment
	gdt_flush(&gdtp);
}

/* Print all the info from the gdt basic info and all of the entries
 *
 */
void gdt_print() {
	uint8_t oldcolor = vga.color;

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
	vga.color = oldcolor;
}
