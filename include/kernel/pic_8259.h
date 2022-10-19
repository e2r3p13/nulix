/* include/kernel/pic_8259.h
 *
 * Manage hardware interrupts through the 8259 PIC.
 *
 * created: 2022/10/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/19 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef PIC_8259_H
#define PIC_8259_H

/* Define new value to remap PIC's IRQs.
 * Since we are in protected mode we need to map IRQs from 32 to 47 instead
 * of 0 to 15 in real mode to avoid conflict with software interrupts. 
 * */
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

void	pic_8259_init(int pic1, int pic2);
void 	pic_8259_eoi();

#endif
