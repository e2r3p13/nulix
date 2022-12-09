// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/nsh/builtins/registers_internal.h
 *
 * Get all x86 registers.
 *
 * created: 2022/12/09 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/09 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef REGISTERS_INTERAL_H
#define REGISTERS_INTERAL_H

#define PRINT(reg, var, format) \
	reg(var); kprintf(format, var)

/* General registers
 */
#define EAX(x) \
	__asm__ ("mov %%eax, %0" : "=r"(x)::)
#define EBX(x) \
	__asm__ ("mov %%ebx, %0" : "=r"(x)::)
#define ECX(x) \
	__asm__ ("mov %%ecx, %0" : "=r"(x)::)
#define EDX(x) \
	__asm__ ("mov %%edx, %0" : "=r"(x)::)

/* Segment registers
 */
#define CS(x) \
	__asm__ ("mov %%cs, %0" : "=r"(x)::)
#define DS(x) \
	__asm__ ("mov %%ds, %0" : "=r"(x)::)
#define ES(x) \
	__asm__ ("mov %%es, %0" : "=r"(x)::)
#define FS(x) \
	__asm__ ("mov %%fs, %0" : "=r"(x)::)
#define GS(x) \
	__asm__ ("mov %%gs, %0" : "=r"(x)::)
#define SS(x) \
	__asm__ ("mov %%ss, %0" : "=r"(x)::)

/* Index and pointers registers
 */
#define ESI(x) \
	__asm__ ("mov %%esi, %0" : "=r"(x)::)
#define EDI(x) \
	__asm__ ("mov %%edi, %0" : "=r"(x)::)
#define EBP(x) \
	__asm__ ("mov %%ebp, %0" : "=r"(x)::)
#define EIP(x) \
	__asm__ ("mov $., %0" : "=r"(x)::)
#define ESP(x) \
	__asm__ ("mov %%esp, %0" : "=r"(x)::)

/* Indicator
 */
#define EFLAGS(x) \
	__asm__ ("pushf\n\t" "pop %0" : "=r"(x)::)

/* Control registers
 */
#define CR0(x) \
	__asm__ ("mov %%cr0, %0" : "=r"(x)::)
#define CR3(x) \
	__asm__ ("mov %%cr3, %0" : "=r"(x)::)

#endif
