// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/panic.h
 *
 * Insert file description here
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/29 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef PANIC_H
#define PANIC_H

#define LOAD_PANIC_STACK \
	__asm__ volatile (\
			"cli\n" \
			"pushal\n"\
			"pusha\n"\
			"pushf\n"\
			)

typedef struct {
	uint32_t	eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t	cs, ds, es, fs, gs, ss;
	uint32_t 	eip;
	uint32_t	eflags;
	uint32_t	cr0, cr2, cr3;

}__attribute__ ((packed)) 	t_panic_frame;

void panic();

#endif
