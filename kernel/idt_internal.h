// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* idt_internal.h
 *
 * Declaration of idt internal structures and functions.
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
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

#endif
