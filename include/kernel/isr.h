// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/isr.h
 *
 * ISR header
 *
 * created: 2023/01/24 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include <kernel/registers.h>

struct int_frame {
	uint32_t	eip;
	uint32_t	cs;
	uint32_t	flags;
	uint32_t	sp, ss;

}__attribute__ ((packed));

typedef struct int_frame t_int_frame;

#define ISR(n) \
extern void *isr_##n; \

#define IRQ(n) \
extern void *irq_##n; \


void isr_handler(int isrnum, uint32_t ring, uint32_t error, struct registers *regs);

ISR(0);
ISR(1);
ISR(2);
ISR(3);
ISR(4);
ISR(5);
ISR(6);
ISR(7);
ISR(8);
ISR(9);
ISR(10);
ISR(11);
ISR(12);
ISR(13);
ISR(14);
ISR(16);
ISR(17);
ISR(18);
ISR(19);
ISR(20);

IRQ(0);
IRQ(1);

#endif
