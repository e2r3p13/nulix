// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* isr.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/24 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/pic_8259.h>
#include <kernel/panic.h>
#include <kernel/registers.h>

#include "idt_internal.h"

/* Debug function to print all interrupt frame.
 *
 * @arg(int_frame): interrupt frame structure.
 */
static void print_int_frame(t_int_frame *int_frame, uint32_t *errcode) {
	kprintf("frame interupt: %p\n", int_frame);
	kprintf("     eip: %x\n", int_frame->eip);
	kprintf("      cs: %x\n", int_frame->cs);
	kprintf("     flg: %x\n", int_frame->flags);
	kprintf("      sp: %x\n", int_frame->sp);
	kprintf("      ss: %x\n", int_frame->ss);
	if (errcode)
	kprintf("     err: %x\n", *errcode);
}

static void print_regs(struct registers *regs) {
	kprintf("regs addr: %p\n", regs);
	kprintf(" ss: %x", regs->ss);
	kprintf(" cs: %x", regs->cs);
	kprintf(" ds: %x", regs->ds);
	kprintf(" es: %x", regs->es);
	kprintf(" fs: %x", regs->fs);
	kprintf(" gs: %x\n", regs->gs);
	kprintf(" flg: %x\n", regs->flg);
	kprintf(" esp: %8x", regs->esp);
	kprintf(" ebp: %8x\n", regs->ebp);
	kprintf(" eip: %8x\n", regs->eip);
	kprintf(" esi: %8x", regs->esi);
	kprintf(" edi: %8x\n", regs->edi);
	kprintf(" eax: %8x", regs->eax);
	kprintf(" ebx: %8x", regs->ebx);
	kprintf(" ecx: %8x", regs->ecx);
	kprintf(" edx: %8x\n", regs->edx);
}

void isr_handler(int isrnum, uint32_t ring, uint32_t error, struct registers *regs) {
	switch (isrnum) {
		case 0:
			kprintf("Divide error\n");
			break;
		default:
			kprintf("isr_handler %d\n", isrnum);
			kprintf("ring: %d\n", ring);
			kprintf("error: %d\n", error);
			print_regs(regs);
	}
}


/* DIVIDE ERROR (0)
 * Occure when division by 0 happen
 *
 * @arg(int_frame): interrupt frame structure.
 */
void divide_error_handler(t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;

	kprintf("DIVIDE ERROR\n");
	print_int_frame(int_frame, NULL);

	RESET_INTERRUPT_STACK;
}

/* OVERFLOW (4)
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void overflow_handler(t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;

	kprintf("OVERFLOW\n");
	print_int_frame(int_frame, NULL);

	RESET_INTERRUPT_STACK;
}

/* DOUBLE FAULT(8)
 * Occure when a fault happen in a interrupt/exception or when no handle functions
 * is in the idt.
 *
 * @arg(int_frame): interrupt frame structure.
 * @arg(error_code): code d'erreur.
 */
__attribute__ ((interrupt)) void double_fault_handler(t_int_frame *int_frame, uint32_t error_code)
{
	LOAD_INTERRUPT_STACK;

	kprintf("DOUBLE FAULT\n");
	PANIC_INTERRUPT(int_frame);

	RESET_INTERRUPT_STACK;
}

#define PRINT(reg, var, format) \
	reg(var); kprintf(format, var)
#define CR3(x) \
	__asm__ ("mov %%cr3, %0" : "=r"(x)::)
#define CR2(x) \
	__asm__ ("mov %%cr2, %0" : "=r"(x)::)

/* PAGE FAULT(14)
 * Occure when a fault happen in a interrupt/exception or when no handle functions
 * is in the idt.
 *
 * @arg(int_frame): interrupt frame structure.
 * @arg(error_code): code d'erreur.
 */
__attribute__ ((interrupt)) void page_fault_handler(t_int_frame *int_frame, uint32_t error_code)
{
	LOAD_INTERRUPT_STACK;

	kprintf("PAGE FAULT\n");
	PANIC_INTERRUPT(int_frame);

	RESET_INTERRUPT_STACK;
}

/* GP FAULT(13)
 * Occure when a fault happen in a interrupt/exception or when no handle functions
 * is in the idt.
 *
 * @arg(int_frame): interrupt frame structure.
 * @arg(error_code): code d'erreur.
 */
__attribute__ ((interrupt)) void gp_fault_handler(t_int_frame *int_frame, uint32_t error_code)
{
	LOAD_INTERRUPT_STACK;

	kprintf("GP FAULT\n");
	PANIC_INTERRUPT(int_frame);

	RESET_INTERRUPT_STACK;
}
/* TIMER HANDLER (32)
 * Occurs when 8259 PIC receive an IRQ0 (timer hardware interrupt)
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void timer_handler(__attribute__((unused)) t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;
	pic_8259_eoi(IRQ_TM);
	RESET_INTERRUPT_STACK;
}

/* KEYBOARD HANDLER (33)
 * Occurs when 8259 PIC receive an IRQ1 (keyboard hardware interrupt)
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void keyboard_handler(__attribute__((unused)) t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;
	pic_8259_eoi(IRQ_KB);
	RESET_INTERRUPT_STACK;
}
