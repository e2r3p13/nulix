// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/info.c
 *
 * Info builtin file
 *
 * created: 2022/12/09 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/17 - xlmod <glafond-@student.42.fr>
 */ 

#include <stdint.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/kpm.h>
#include <kernel/screenbuf.h>
#include <kernel/stdlib.h>

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

extern struct screenbuf *sb_current;

#define PRINT(reg, var, format) \
	reg(var); kprintf(format, var)

#define BLTNAME "info"

static inline void usage() {
	kprintf("Usage: " BLTNAME " [gdt/idt/stack/buddy/registers]\n");
}

static void info_registers() {
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

static void info_idt() {
	kprintf("INFO IDT\n");
	kprintf("Base:  %8p | Limit: %8p\n", idtp.base, idtp.limit);
	kprintf("Size = %u bytes\n", sizeof(idt));	
}

static void info_buddy_print_order(int order) {
	uint8_t oldcolor = sb_get_color(sb_current);
	kprintf("order %u(size of %u)\n", order, buddy->orders[order].size);
	for (size_t i = 0; i < buddy->orders[order].size; i++) {
		if (i % 32 == 0) {
			kprintf("\n%4x  ", i);
		}
		bitmap_t val = buddy->orders[order].bitmap[i];
		if (val == 0) {
			sb_set_bg(sb_current, SB_COLOR_WHITE);
			kprintf("  ");
		} else if (val == 0xff) {
			sb_set_bg(sb_current, SB_COLOR_RED);
			kprintf("  ");
		} else {
			sb_set_bg(sb_current, SB_COLOR_BROWN);
			kprintf("%2x", val);
		}
		sb_set_color(sb_current, oldcolor);
	}
	sb_set_color(sb_current, oldcolor);
	kprintf("\n");
}

static void info_buddy(int order) {
	kprintf("INFO BUDDY\n");
	kprintf("buddy address:        %p\n", buddy);
	kprintf("buddy size:           %u KB\n", buddy->size / 1024);
	kprintf("orders address:       %p\n", buddy->orders[0].bitmap);
	kprintf("frame number:         %x\n", buddy->nframes);
	kprintf("memory size:          %u KB\n", buddy->nframes << 2);
	if (order >= 0)
		info_buddy_print_order(order);
}

static void info_stack() {
	kprintf("INFO STACK\n");
	kprintf("Top:   %8p | Bottom : %8p\n", &stack_top, &stack_bottom);
	kprintf("Size = %d bytes\n", (void *)&stack_top - (void *)&stack_bottom);
}

static void info_gdt() {
	kprintf("INFO GDT\n");
	kprintf("Base:  %8p | Limit: %8p\n", gdtp.base, gdtp.limit);
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

int info(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return -1;
	}
	if (!strcmp(argv[1], "gdt")) {
		info_gdt();
	} else if (!strcmp(argv[1], "stack")) {
		info_stack();
	} else if (!strcmp(argv[1], "buddy")) {
		if (argc > 2) {
			char *ptr;
			int order = strtol(argv[2], &ptr, 0);
			if (*ptr || order < 0 || order >= KPM_NORDERS) {
				kprintf(BLTNAME ": order number not formatted correctly\n");
				return -1;
			}
			info_buddy(order);
		} else {
			info_buddy(-1);
		}
	} else if (!strcmp(argv[1], "idt")) {
		info_idt();
	} else if (!strcmp(argv[1], "registers")) {
		info_registers();
	} else {
		kprintf(BLTNAME ": '%s' doesn't exist.\n", argv[1]);
		return -1;
	}
	return 0;
}
