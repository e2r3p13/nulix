#include <kernel/string.h>
#include <kernel/vga.h>
#include <kernel/print.h>

void kernel_main(void) {

	VGA_initialize();
	VGA_writestring("The anwser is 42\n");
}
