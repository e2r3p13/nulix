// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/irq_internal.h
 *
 * 
 *
 * created: 2022/10/20 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/20 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef IRQ_INTERNAL_H
#define IRQ_INTERNAL_H

#include "idt_internal.h"

__attribute__ ((interrupt)) void keyboard_handler(t_int_frame *int_frame);

#endif
