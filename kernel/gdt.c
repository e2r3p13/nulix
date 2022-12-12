// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.c
 *
 * Fill the gdt entries and load it.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <kernel/gdt.h>

#include "gdt_internal.h"

extern void gdt_flush();

/* Pointer to the gdt. */
t_gdt_entry *gdt = (t_gdt_entry *)GDT_BASE_ADDRESS;

/* Store a pointer to the gdt and its size. */
t_gdt_ptr gdtp;

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
	gdt[num].granularity |= (gran & 0xFF);
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
