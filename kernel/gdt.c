// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.c
 *
 * Fill the gdt entries and load it.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <stdint.h>
#include <kernel/gdt.h>

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
	gdtp.limit = (sizeof(t_gdt_entry) * 5) - 1;
	gdtp.base = (uint32_t)gdt;
	gdt_set_gate(0,	0,	0,	0,	0);	// NULL descriptor
	gdt_set_gate(1,	0,	0xFFFF,	0x9A,	0xCF);	// Kernel Mode Code Segment
	gdt_set_gate(2,	0,	0xFFFF,	0x92,	0xCF);	// Kernel Mode Data Segment
	gdt_set_gate(3,	0,	0xFFFF,	0xFA,	0xCF);	// User Mode Code Segment
	gdt_set_gate(4,	0,	0xFFFF,	0xF2,	0xCF);	// User mode Data Segment
	gdt_flush(&gdtp);
}

