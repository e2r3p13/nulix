/* kernel/8259-pic.c
 *
 * Manage Programmable Interrupt Controler.
 *
 * created: 2022/10/19 - mrxx0 <chcoutur-@student.42.fr>
 * updated: 2022/10/19 - mrxx0 <chcoutur-@student.42.fr>
 */

#include <kernel/pic_8259.h>
#include <kernel/port.h>

void pic_8259_init(int pic1, int pic2)
{
	/* send ICW1 */
	port_write(PIC1, ICW1);
	port_write(PIC2, ICW1);
	/* send ICW2 */
	port_write(PIC1 + 1, pic1);
	port_write(PIC2 + 1, pic2);
	/* send ICW3 */
	port_write(PIC1 + 1, 4);
	port_write(PIC2 + 1, 2);
	/* send ICW4 */
	port_write(PIC1 + 1, ICW4);
	port_write(PIC2 + 1, ICW4);
	/* disable IRQs */
	port_write(PIC1 + 1, 0xFF);
	port_write(PIC2 + 1, 0xFF);
}

void pic_8259_eoi()
{
	port_write(PIC2, PIC1);
	port_write(PIC1, PIC1);
	/* In case we don't handle start/stop interrupts in our handling function we 
	 * need to start them after EOI.
	 * */
}
