// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* isr.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/pic_8259.h>
#include <kernel/panic.h>
#include <kernel/registers.h>

#include "idt_internal.h"

static const char *intname[] = {
	"Divide-by-zero Error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown",
	"x87 Floating-Point Exception",
	"Alignement Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Security Exception",
	"Unknown",
};

static void intprint(int isrnum, uint32_t ring, uint32_t error, struct registers *regs) {
	kprintf("[%d] %s\n", isrnum, intname[isrnum]);
	kprintf("ring: %d\n", ring);
	kprintf("error: %d\n", error);
	registers_print(regs);
}

static void page_fault_handler(uint32_t ring, uint32_t error, struct registers *regs) {
	PANIC_INTERRUPT(regs);
}

void isr_handler(int isrnum, uint32_t ring, uint32_t error, struct registers *regs) {
	switch (isrnum) {
		case 8:
			PANIC_INTERRUPT(regs);
			break;
		case 14:
			page_fault_handler(ring, error, regs);
			break;
		default:
			if (isrnum < 32)
				intprint(isrnum, ring, error, regs);
			break;
	}
}
