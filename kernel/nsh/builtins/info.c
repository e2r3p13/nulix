// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/info.c
 *
 * Insert file description here
 *
 * created: 2022/12/09 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/13 - glafond- <glafond-@student.42.fr>
 */ 

#include <stdint.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/kpm.h>

#include "../../gdt_internal.h"
#include "../../idt_internal.h"
#include "registers_internal.h"

extern t_gdt_entry *gdt;
extern t_gdt_ptr gdtp;
extern void *stack_bottom;
extern void *stack_top;
extern buddy_t *buddy;
extern t_idt_entry idt[256];
extern t_idt_ptr idtp;

#define PRINT(reg, var, format) \
	reg(var); kprintf(format, var)

void info_registers()
{
	uint32_t reg;
	PRINT(EAX, reg, "eax : %8p\n");
	PRINT(EBX, reg, "ebx : %8p\n");
	PRINT(ECX, reg, "ecx : %8p\n");
	PRINT(EDX, reg, "edx : %8p\n");
	PRINT(CS, reg, "cs : %8p\n");
	PRINT(DS, reg, "ds : %8p\n");
	PRINT(ES, reg, "es : %8p\n");
	PRINT(GS, reg, "gs : %8p\n");
	PRINT(SS, reg, "ss : %8p\n");
	PRINT(ESI, reg, "esi : %8p\n");
	PRINT(EDI, reg, "edi : %8p\n");
	PRINT(EBP, reg, "ebp : %8p\n");
	PRINT(EIP, reg, "eip : %8p\n");
	PRINT(ESP, reg, "esp : %8p\n");
	PRINT(EFLAGS, reg, "eflags : %b\n");
	PRINT(CR0, reg, "cr0 : %8b\n");
	PRINT(CR3, reg, "cr3 : %8p\n");
}
/* Print all the info from the idt basic info and all of the entries
 *
 */
void info_idt()
{
	kprintf("INFO IDT\n");
	kprintf("Base : %8p | Limit: %8p\n", idtp.base, idtp.limit);
	kprintf("Size = %u bytes\n", sizeof(idt));	
}

/* Print all the info from the buddy allocator
 *
 */
void info_buddy()
{
	kprintf("INFO BUDDY\n");
	kprintf("buddy address:        %p\n", buddy);
	kprintf("buddy size:           %u KB\n", buddy->size * 1024);
	kprintf("orders address:       %p\n", buddy->orders);
	kprintf("block number:         %x\n", buddy->nframes);
	kprintf("total blocks size:    %u KB\n\n", (buddy->nframes << 2));
}

void info_stack()
{
	kprintf("INFO STACK\n");
	kprintf("Top : %8p | Bottom : %8p\n", &stack_top, &stack_bottom);
	kprintf("Size : %d bytes\n", (void *)&stack_top - (void *)&stack_bottom);
}

/* Print all the info from the gdt basic info and all of the entries
 *
 */
void info_gdt()
{
	kprintf("INFO GDT\n");
	kprintf("Base : %8p | Limit: %8p\n", gdtp.base, gdtp.limit);
	kprintf("Size = %u bytes\n", sizeof(gdt) * GDT_SIZE);
	kprintf("BASE LOW    BASE MID    BASE HIGH   LIMIT LOW   GRAN        ACC\n");
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

int info(int argc, char **argv)
{
	if (argc < 2) {
		kprintf("info" ": Not enough arguments.\n");
		kprintf("Usage : info [gdt/idt/stack/buddy/registers]\n");
		return -1;
	}
	if (!strcmp(argv[1], "gdt")) {
		info_gdt();
	} else if (!strcmp(argv[1], "stack")) {
		info_stack();
	} else if (!strcmp(argv[1], "buddy")) {
		info_buddy();
	} else if (!strcmp(argv[1], "idt")) {
		info_idt();
	} else if (!strcmp(argv[1], "registers")) {
		info_registers();
	} else {
		kprintf("info : '%s' doesn't exist.\n", argv[1]);
		kprintf("Usage : info [gdt/idt/stack/buddy/registers]\n");
		return -1;
	}
	return 0;
}
