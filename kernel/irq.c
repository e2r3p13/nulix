// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* irq.c
 *
 * Definition of IRQ (Interrupt Request) to handle non-software interrupts
 * such as keyboard.
 *
 * updated: 2022/10/20 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/20 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/pic_8259.h>
#include <kernel/print.h>

#include "idt_internal.h"

/* KEYBOARD HANDLER (33)
 * Occurs when 8259 PIC receive an IRQ1 (keyboard hardware interrupt)
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void keyboard_handler(t_int_frame *int_frame)
{
	kprintf("KEYBOARD IRQ\n");
	LOAD_INTERRUPT_STACK;
	pic_8259_eoi(1);
	RESET_INTERRUPT_STACK;
}
