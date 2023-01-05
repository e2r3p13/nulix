// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* idt.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/05 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <kernel/print.h>

#include "idt_internal.h"

t_idt_entry *idt = (t_idt_entry *)0x5000;
t_idt_ptr idtp;

// Set up a descriptor
void idt_set_descriptor(uint8_t idt_index, void *isr_addr, uint8_t flags) {
	t_idt_entry *descriptor = &idt[idt_index];

	descriptor->base_low = (uint32_t)isr_addr & 0xFFFF;	// Low 16 bits of isr_addr
	descriptor->selector = 0x08; 						// GDT Offset ; Kernel code segment containing this isr
	descriptor->zero = 0;								// Reserved : always set to 0
	descriptor->type_attributes = flags;				// Type & attributes (INT_GATE, TRAP_GATE...)
	descriptor->base_high = (uint32_t)isr_addr >> 16;	// High 16 bitd of isr_addr
}

void idt_init() {
	idtp.limit = sizeof(t_idt_entry) * 256;
	idtp.base = (uint32_t)idt;

	idt_set_descriptor(ISR_DE, divide_error_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(ISR_OF, overflow_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(ISR_DF, double_fault_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(ISR_GF, gp_fault_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(ISR_PF, page_fault_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(ISR_TM, timer_handler, INT_GATE_FLAGS);
	idt_set_descriptor(ISR_KB, keyboard_handler, INT_GATE_FLAGS);

	__asm__ volatile ("lidt %0" :: "m"(idtp));
	__asm__ volatile ("sti");
}
