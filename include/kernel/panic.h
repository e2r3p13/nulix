// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/panic.h
 *
 * Insert file description here
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef PANIC_H
#define PANIC_H

#define LOAD_PANIC_STACK \
	__asm__ volatile (\
			"cli\n" \
			"pusha\n" \
			)

#define RESET_PANIC_STACK \
	__asm__ volatile (\
			"popa\n" \
			)

typedef struct {
	uint32_t	eax, ebx, ecx, edx;
	uint32_t	cs, ds, es, fs, gs, ss;
	uint32_t	esi, edi, ebp, eip, esp;
	uint32_t	eflags;
	uint32_t	cr0, cr2, cr3;

}__attribute__ ((packed)) 	t_panic_frame;

void panic();

#endif
