// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/registers.c
 *
 *	Registers functions 
 *
 * created: 2023/01/25 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/registers.h>
#include <kernel/print.h>

void registers_print(struct registers *regs) {
	kprintf("EAX=%8x", regs->eax);
	kprintf(" EBX=%8x", regs->ebx);
	kprintf(" ECX=%8x", regs->ecx);
	kprintf(" EDX=%8x\n", regs->edx);
	kprintf("ESI=%8x", regs->esi);
	kprintf(" EDI=%8x", regs->edi);
	kprintf(" EBP=%8x", regs->ebp);
	kprintf(" ESP=%8x\n", regs->esp);
	kprintf("EIP=%8x", regs->eip);
	kprintf(" EFL=%8x\n", regs->flg);
	kprintf("SS =%4x\n", regs->ss);
	kprintf("CS =%4x\n", regs->cs);
	kprintf("DS =%4x\n", regs->ds);
	kprintf("ES =%4x\n", regs->es);
	kprintf("FS =%4x\n", regs->fs);
	kprintf("GS =%4x\n", regs->gs);
	kprintf("CR0=%8x", regs->cr0);
	kprintf(" CR2=%8x", regs->cr2);
	kprintf(" CR3=%8x\n", regs->cr3);
}
