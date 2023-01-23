// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* panic.c
 *
 * Panic screen handler
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/23 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/panic.h>

static void panic_print_flag() {
	kprintf("###########################################################################\n");
	kprintf("#  ___  ___  _  _  ___   ___                                              #\n");
	kprintf("# | _ \\/   \\| \\| ||_ _| / __|                                             #\n");
	kprintf("# |  _/| - || .  | | | | (__                                              #\n");
	kprintf("# |_|  |_|_||_|\\_||___| \\___|                                             #\n");
	kprintf("#                                                                         #\n");
}

void panic_print_frame(t_panic_frame *panic_frame) {
	panic_print_flag();
	kprintf("# EAX=%8x ", panic_frame->eax);
	kprintf("EBX=%8x ", panic_frame->ebx);
	kprintf("ECX=%8x ", panic_frame->ecx);
	kprintf("EDX=%8x", panic_frame->edx);
	kprintf("                     #\n");
	kprintf("# ESI=%8x ", panic_frame->esi);
	kprintf("EDI=%8x ", panic_frame->edi);
	kprintf("EBP=%8x ", panic_frame->ebp);
	kprintf("ESP=%8x", panic_frame->esp);
	kprintf("                     #\n");
	kprintf("# EIP=%8x ", panic_frame->eip - 4);
	kprintf("EFL=%8x", panic_frame->eflags);
	kprintf("                                               #\n");
	kprintf("# ES= %4x                                                                #\n", panic_frame->es);
	kprintf("# CS= %4x                                                                #\n", panic_frame->cs);
	kprintf("# SS= %4x                                                                #\n", panic_frame->ss);
	kprintf("# DS= %4x                                                                #\n", panic_frame->ds);
	kprintf("# FS= %4x                                                                #\n", panic_frame->fs);
	kprintf("# GS= %4x                                                                #\n", panic_frame->gs);
	kprintf("# CR0=%8x ", panic_frame->cr0);
	kprintf("CR2=%8x ", panic_frame->cr2);
	kprintf("CR3=%8x", panic_frame->cr3);
	kprintf("                                  #\n");
	kprintf("#                                                                         #\n");
	kprintf("###########################################################################\n");
}

void panic_print_interrupt_frame(t_panic_frame *panic_frame, t_int_frame *int_frame) {
	panic_print_flag();
	kprintf("# EIP=%8x ", int_frame->eip);
	kprintf("EFL=%8x", int_frame->flags);
	kprintf("                                               #\n");
	kprintf("# ESP=%8x                                                            #\n", int_frame->sp);
	kprintf("# CS= %4x                                                                #\n", int_frame->cs);
	kprintf("# SS= %4x                                                                #\n", int_frame->ss);
	kprintf("# CR0=%8x ", panic_frame->cr0);
	kprintf("CR2=%8x ", panic_frame->cr2);
	kprintf("CR3=%8x", panic_frame->cr3);
	kprintf("                                  #\n");
	kprintf("#                                                                         #\n");
	kprintf("###########################################################################\n");
}
