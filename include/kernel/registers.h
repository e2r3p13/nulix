// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/registers.h
 *
 * Registers header
 *
 * created: 2023/01/24 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

struct registers {
	uint32_t ss;
	uint32_t cs;
	uint32_t ds;
	uint32_t es;
	uint32_t fs;
	uint32_t gs;
	uint32_t flg;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
	uint32_t esi;
	uint32_t edi;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t cr0;
	uint32_t cr2;
	uint32_t cr3;
} __attribute__ ((packed));

#define REG_SAVE(regs) \
do { \
	asm("movl %%eax, %0" : "=rm" ( (regs)->eax )); \
	asm("popl %0" : "=rm" ((regs)->eip)); \
	asm("movl %%ecx, %0" : "=rm" ( (regs)->ecx )); \
	asm("movl %%edx, %0" : "=rm" ( (regs)->edx )); \
	asm("movl %%ebx, %0" : "=rm" ( (regs)->ebx )); \
	asm("movl %%esp, %0" : "=rm" ( (regs)->esp )); \
	asm("movl %%ebp, %0" : "=rm" ( (regs)->ebp )); \
	asm("movl %%esi, %0" : "=rm" ( (regs)->esi )); \
	asm("movl %%edi, %0" : "=rm" ( (regs)->edi )); \
	asm("pushf\n\t" "pop %0" : "=rm" ((regs)->flg )); \
	asm("mov %%cs, %0" : "=rm" ( (regs)->cs )); \
	asm("mov %%ds, %0" : "=rm" ( (regs)->ds )); \
	asm("mov %%es, %0" : "=rm" ( (regs)->es )); \
	asm("mov %%fs, %0" : "=rm" ( (regs)->fs )); \
	asm("mov %%gs, %0" : "=rm" ( (regs)->gs )); \
	asm("mov %%ss, %0" : "=rm" ( (regs)->ss )); \
	asm("movl %%cr0, %0" : "=rm" ( (regs)->cr0 )); \
	asm("movl %%cr2, %0" : "=rm" ( (regs)->cr2 )); \
	asm("movl %%cr3, %0" : "=rm" ( (regs)->cr3 )); \
} while (0)

void registers_print(struct registers *regs);

#endif
