// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/panic.h
 *
 * Insert file description here
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/23 - glafond- <glafond-@student.42.fr>
 */

#ifndef PANIC_H
#define PANIC_H

#include "../../kernel/idt_internal.h"

typedef struct {
	uint32_t	eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t	cs, ds, es, fs, gs, ss;
	uint32_t 	eip;
	uint32_t	eflags;
	uint32_t	cr0, cr2, cr3;
}__attribute__ ((packed)) 	t_panic_frame;

void panic_print_frame(t_panic_frame *panic_frame);
void panic_print_interrupt_frame(t_panic_frame *panic_frame,
		t_int_frame *int_frame);

#define PANIC \
do { \
	t_panic_frame __panic_frame = {0}; \
	asm("cli"); \
	asm("mov %%eax, %0" : "=rm" ( __panic_frame.eax )); \
	asm("1: movl $1b, %0" : "=rm" (__panic_frame.eip)); \
	asm("mov %%ecx, %0" : "=rm" ( __panic_frame.ecx )); \
	asm("mov %%edx, %0" : "=rm" ( __panic_frame.edx )); \
	asm("mov %%ebx, %0" : "=rm" ( __panic_frame.ebx )); \
	asm("mov %%esp, %0" : "=rm" ( __panic_frame.esp )); \
	asm("mov %%ebp, %0" : "=rm" ( __panic_frame.ebp )); \
	asm("mov %%esi, %0" : "=rm" ( __panic_frame.esi )); \
	asm("mov %%edi, %0" : "=rm" ( __panic_frame.edi )); \
	asm("pushf\n\t" "pop %0" : "=rm" (__panic_frame.eflags )); \
	asm("mov %%cs, %0" : "=rm" ( __panic_frame.cs )); \
	asm("mov %%ds, %0" : "=rm" ( __panic_frame.ds )); \
	asm("mov %%es, %0" : "=rm" ( __panic_frame.es )); \
	asm("mov %%fs, %0" : "=rm" ( __panic_frame.fs )); \
	asm("mov %%gs, %0" : "=rm" ( __panic_frame.gs )); \
	asm("mov %%ss, %0" : "=rm" ( __panic_frame.ss )); \
	asm("mov %%cr0, %0" : "=rm" ( __panic_frame.cr0 )); \
	asm("mov %%cr2, %0" : "=rm" ( __panic_frame.cr2 )); \
	asm("mov %%cr3, %0" : "=rm" ( __panic_frame.cr3 )); \
	panic_print_frame(&__panic_frame); \
	asm("hlt"); \
} while (1)

#define PANIC_INTERRUPT(int_frame) \
do { \
	t_panic_frame __panic_frame = {0}; \
	asm("cli"); \
	asm("mov %%cr0, %0" : "=rm" ( __panic_frame.cr0 )); \
	asm("mov %%cr2, %0" : "=rm" ( __panic_frame.cr2 )); \
	asm("mov %%cr3, %0" : "=rm" ( __panic_frame.cr3 )); \
	panic_print_interrupt_frame(&__panic_frame, int_frame); \
	asm("hlt"); \
} while (1)

#endif
