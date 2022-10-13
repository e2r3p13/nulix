/* kernel/kernel.c
 *
 * Entry of the kernel with init function and kernel_main.
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/vga.h>

extern void *stack_top;
extern void *stack_bottom;

/* Initialize all descriptor tables (gdt, idt, ...)
 */
void init_descriptor_tables() {
	gdt_init();
}

void kernel_main(void) {
	init_descriptor_tables();

	VGA_initialize();
	kprintf("stack top: %p     stack bottom: %p\n", &stack_top, &stack_bottom);
	kprintf("before interrupt\n");
	kprintf("end\n");

}
