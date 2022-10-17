/* kernel/idt_internal.h
 *
 * Declaration of idt internal structures and functions.
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/18 - xlmod <glafond-@student.42.fr>
 */

#ifndef IDT_INTERNAL_H
#define IDT_INTERNAL_H

#include <stdint.h>

// source Intel Software Developer Manual, Volume 3-A
//P = 1bit, DPL = 2bits, S = 1bit, Type = 4bits
#define TRAP_GATE_FLAGS 		0x8F
#define INT_GATE_FLAGS 			0x8E
#define INT_GATE_FLAGS_USER 	0xEE

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
			"mov %ax, %gs\n")

/* Macro to reset the segment registers to their original values.
 */
#define RESET_INTERRUPT_STACK \
	__asm__ volatile (\
			"pop %eax\n"\
			"mov %ds, %ax\n"\
			"mov %es, %ax\n"\
			"mov %fs, %ax\n"\
			"mov %gs, %ax\n"\
			"sti\n")


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
typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
}__attribute__ ((packed)) 	t_int_frame;

/* ISR
 * Interrupt/Exception handler.
 */
__attribute__ ((interrupt)) void divide_error_handler(t_int_frame *int_frame);
__attribute__ ((interrupt)) void overflow_handler(t_int_frame *int_frame);
__attribute__ ((interrupt)) void double_fault_handler(t_int_frame *int_frame, uint32_t error_code);

#endif
