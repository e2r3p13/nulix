// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* idt.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <kernel/kmalloc.h>
#include <kernel/isr.h>

#include "idt_internal.h"

t_idt_entry *idt;
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
	size_t idtbytes = sizeof(t_idt_entry) * 256;
	idt = (t_idt_entry *)kmalloc(idtbytes, KMF_NOFAIL);
	idtp.limit = idtbytes;
	idtp.base = (uint32_t)idt;

	idt_set_descriptor(0,	&isr_0,		INT_GATE_FLAGS);
	idt_set_descriptor(1,	&isr_1,		INT_GATE_FLAGS);
	idt_set_descriptor(2,	&isr_2,		INT_GATE_FLAGS);
	idt_set_descriptor(3,	&isr_3,		INT_GATE_FLAGS);
	idt_set_descriptor(4,	&isr_4,		INT_GATE_FLAGS);
	idt_set_descriptor(5,	&isr_5,		INT_GATE_FLAGS);
	idt_set_descriptor(6,	&isr_6,		INT_GATE_FLAGS);
	idt_set_descriptor(7,	&isr_7,		INT_GATE_FLAGS);
	idt_set_descriptor(8,	&isr_8,		INT_GATE_FLAGS);
	idt_set_descriptor(9,	&isr_8,		INT_GATE_FLAGS);
	idt_set_descriptor(10,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(11,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(12,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(13,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(14,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(16,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(17,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(18,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(19,	&isr_14,	INT_GATE_FLAGS);
	idt_set_descriptor(20,	&isr_14,	INT_GATE_FLAGS);

	idt_set_descriptor(32 + 0, &irq_0,	INT_GATE_FLAGS);
	idt_set_descriptor(32 + 1, &irq_1,	INT_GATE_FLAGS);

	__asm__ volatile ("lidt %0" :: "m"(idtp));
	__asm__ volatile ("sti");
}
