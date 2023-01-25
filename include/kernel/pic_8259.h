/* include/kernel/pic_8259.h
 *
 * Manage hardware interrupts through the 8259 PIC.
 *
 * created: 2022/10/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/19 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef PIC_8259_H
#define PIC_8259_H

#include <stdint.h>

/* Define new value to remap PIC's IRQs.
 * Since we are in protected mode we need to map IRQs from 32 to 47 instead
 * of 0 to 15 in real mode to avoid conflict with software interrupts. 
 * */

/* IO base address for PIC1 and PIC2 */
#define PIC1 0x20
#define PIC2 0xA0
/* Offset for PIC1 and PIC2 */
#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28
/* Address to access data on PIC1 and PIC2 */
#define PIC1_DATA (PIC1+1)
#define PIC2_DATA (PIC2+1)
/* Address to send command to PIC1 and PIC2 */
#define PIC1_CMD PIC1
#define PIC2_CMD PIC2
/* Code to signal end of interrupt to the PIC */
#define PIC_EOI PIC1
/* Initialization Command Words to initialize PIC1 and PIC2 
 *
 * ICW1	: 	Parameters to control PIC's mode of operation.
 * ICW2 : 	Informs PIC about the base offset in the IDT to transmit an IRQ to
 * 			the CPU.
 * ICW3 :	Information about cascade connection between PIC1 and PIC2.
 * ICW4 : 	Set some additional operation mode for the PIC.s
 * */
#define ICW1 0x11
#define ICW4 0x01

#define IRQ_TM 0
#define IRQ_KB 1

void	pic_8259_init(int pic1, int pic2);
void 	pic_8259_eoi(uint8_t irq_id);

#endif
