/* kernel/isr.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/18 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/print.h>

#include "idt_internal.h"

/* Debug function to print all interrupt frame.
 *
 * @arg(int_frame): interrupt frame structure.
 */
static void print_int_frame(t_int_frame *int_frame) {
	kprintf("frame interupt:\n");
	kprintf("      ds: %x\n", int_frame->ds);
	kprintf("     edi: %x", int_frame->edi);
	kprintf(" esi: %x", int_frame->esi);
	kprintf(" ebp: %x", int_frame->ebp);
	kprintf(" esp: %x\n", int_frame->esp);
	kprintf("     ebx: %x", int_frame->ebx);
	kprintf(" edx: %x", int_frame->edx);
	kprintf(" ecx: %x", int_frame->ecx);
	kprintf(" eax: %x\n", int_frame->eax);
	kprintf("  int_no: %x", int_frame->int_no);
	kprintf("  ec: %x\n", int_frame->err_code);
	kprintf("     eip: %x", int_frame->eip);
	kprintf("  cs: %x", int_frame->cs);
	kprintf("  ef: %x\n", int_frame->eflags);
	kprintf(" useresp: %x", int_frame->useresp);
	kprintf("  ss: %x\n", int_frame->ss);
}

/* DIVIDE ERROR (0)
 * Occure when division by 0 happen
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void divide_error_handler(t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;

	kprintf("DIVIDE ERROR\n");
	print_int_frame(int_frame);
	uint32_t a;
	asm volatile ("mov %%esp, %0" : "=a"(a));
	kprintf("\nesp: %x\n", a);

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
	print_int_frame(int_frame);

	RESET_INTERRUPT_STACK;
}


/* DOUBLE FAULT(0)
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
	print_int_frame(int_frame);
	kprintf("err_code: %x\n", error_code);
	asm volatile ("hlt");

	RESET_INTERRUPT_STACK;
}
