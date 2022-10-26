// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.c
 *
 * Fill the gdt entries and load it.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/26 - mrxx0 <chcoutur@student.42.fr>
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
	gdtp.limit = (sizeof(t_gdt_entry) * 7) - 1;
	gdtp.base = (uint32_t)gdt;
	gdt_set_gate(0,	0,	0,	0,	0);	// NULL descriptor
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
void print_gdt()
{
	kprintf("GDT ADDRESS = 0x00000%x\n", gdt);
	kprintf("GDT ENTRIES : \n");
	kprintf("GDT Base = 0x00000%x\n", gdtp.base);
	kprintf("GDT Limit = 0x00000%x\n", gdtp.limit);
	int i = 0;
	kprintf("BASE LOW | BASE MID | BASE HIGH | LIMIT LOW | GRAN | ACC\n");
	while (i <= 7)
	{
		kprintf("0x00000%x    ", gdt[i].base_low);
		kprintf("0x00000%x    ", gdt[i].base_middle);
		kprintf("0x00000%x    ", gdt[i].base_high);
		kprintf("0x00000%x    ", gdt[i].limit_low);
		kprintf("0x00000%x    ", gdt[i].granularity);
		kprintf("0x00000%x\n", gdt[i].access);
		i++;
	}
}

/* Print the stack from ebp to limit
 *
 * No args at the moment.
 * @ebp is the base pointer from the stack
 * @limit is the mumber of byte printed (one address == 16 bytes) and it's
 * hardcoded. In the future we can set it as parameter.
 *
 */
void print_stack()
{
	uint32_t ebp;
	asm inline volatile ("mov %%ebp, %0" :: "r"(ebp));
	uint32_t limit = 512;
	uint32_t i = 0;
	uint8_t line = 0;
	struct kbd_event evt;

	while (i < limit)
	{
		if (line == 25)
		{
			while (!KBD_poll());
			KBD_getevent(&evt);
			if (evt.type != KEY_PRESSED)
				continue;
			line = 0;
		}
		kprintf("0x%x : ", ebp);
		uint32_t next = ebp + 16;
		uint32_t tmp = ebp;

		while (tmp < next)
		{
			if (*(char*)tmp > 32)
				kprintf("%x ", *(char*)tmp);
			else
				kprintf("00 ");
			tmp++;
		}
		next = ebp + 16;
		tmp = ebp;
		while (tmp < next)
		{
			VGA_setforegroundcolor(VGA_COLOR_BLUE);
			if (*(char*)tmp > 32)
				kprintf("%c", *(char *)tmp);
			else
				kprintf(".");
			tmp++;
		}
		VGA_setforegroundcolor(VGA_COLOR_WHITE);
		kprintf("\n");
		i += 16;
		ebp += 16;
		line++;
	}
}
