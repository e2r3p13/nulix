/* kernel/idt.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/18 - xlmod <glafond-@student.42.fr>
 */
#include <stdint.h>

#include <kernel/print.h>

#include "idt_internal.h"

__attribute__((aligned(0x10)))
static t_idt_entry idt[256];
static t_idt_ptr idtp;

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
	idtp.limit = (uint16_t)sizeof(t_idt_entry) * 256 - 1;
	idtp.base = (uintptr_t)idt;

	idt_set_descriptor(0, divide_error_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(4, overflow_handler, TRAP_GATE_FLAGS);
	idt_set_descriptor(8, double_fault_handler, TRAP_GATE_FLAGS);

	__asm__ volatile ("lidt %0" :: "m"(idtp));
	__asm__ volatile ("sti");
}
