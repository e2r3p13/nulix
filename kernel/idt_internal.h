// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* idt_internal.h
 *
 * Declaration of idt internal structures and functions.
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/24 - glafond- <glafond-@student.42.fr>
 */

#ifndef IDT_INTERNAL_H
#define IDT_INTERNAL_H

#include <stdint.h>
#include <kernel/isr.h>

// source Intel Software Developer Manual, Volume 3-A
//P = 1bit, DPL = 2bits, S = 1bit, Type = 4bits
#define TRAP_GATE_FLAGS 		0x8F
#define INT_GATE_FLAGS 			0x8E
#define INT_GATE_FLAGS_USER 	0xEE

/* Macro to define ISR and IRQ
 * source : https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html
* 'Table 6-1. Protected-Mode Exceptions and Interrupts'
*  We are using the mnemonic from Intel.
 */
/* ISRs are used in the IDT management. */
#define ISR_DE 0
#define ISR_OF 4
#define ISR_DF 8
#define ISR_GF 13
#define ISR_PF 14
#define ISR_TM 32
#define ISR_KB 33

/* ISR_IRQ is the offset to get the id from ISR to IRQ.*/
#define ISR_IRQ 32

/* IRQs are used in the PICs management.*/
#define IRQ_TM ISR_TM - ISR_IRQ
#define IRQ_KB ISR_KB - ISR_IRQ

/* Macro to load kernel data segment in segment registers.
 */
#define LOAD_INTERRUPT_STACK \
	__asm__ volatile (\
			"cli\n"\
			"mov %ds, %ax\n"\
			"push %eax\n"\
			"mov $0x10, %ax\n"\
			"mov %ax, %ds\n"\
			"mov %ax, %es\n"\
			"mov %ax, %fs\n"\
			"mov %ax, %gs\n"\
			"add $4, %esp\n"\
			)

/* Macro to reset the segment registers to their original values.
 */
#define RESET_INTERRUPT_STACK \
	__asm__ volatile (\
			"sub $4, %esp\n"\
			"pop %eax\n"\
			"mov %ax, %ds\n"\
			"mov %ax, %es\n"\
			"mov %ax, %fs\n"\
			"mov %ax, %gs\n"\
			"sti\n"\
			)

/* Entry in the IDT
 */
typedef struct {
	uint16_t	base_low;
	uint16_t	selector;
	uint8_t		zero;
	uint8_t		type_attributes;
	uint16_t	base_high;
}__attribute__ ((packed))	t_idt_entry;

/* Store a pointer to the IDT
 */
typedef struct {
	uint16_t	limit;
	uint32_t	base;
}__attribute__ ((packed))	t_idt_ptr;

/* Store registers when interrupt happen
 */
/* ISR
 * Interrupt/Exception handler.
 */
extern void *divide_error_isr;

void divide_error_handler(t_int_frame *int_frame);
__attribute__ ((interrupt)) void overflow_handler(t_int_frame *int_frame);
__attribute__ ((interrupt)) void double_fault_handler(t_int_frame *int_frame, uint32_t error_code);
__attribute__ ((interrupt)) void gp_fault_handler(t_int_frame *int_frame, uint32_t error_code);
__attribute__ ((interrupt)) void page_fault_handler(t_int_frame *int_frame, uint32_t error_code);

/* IRQ
 * Hardware Interrupt
 */
__attribute__ ((interrupt)) void timer_handler(t_int_frame *int_frame);
__attribute__ ((interrupt)) void keyboard_handler(t_int_frame *int_frame);

#endif
