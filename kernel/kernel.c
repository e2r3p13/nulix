/* kernel/kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/17 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/string.h>
#include <kernel/vga.h>
#include <kernel/keyboard.h>

void kernel_main(void) {
	char c;
	struct kbd_event evt;

	VGA_initialize();
	VGA_setforegroundcolor(VGA_COLOR_GREEN);
	VGA_writestring("42");
	VGA_setforegroundcolor(VGA_COLOR_LIGHT_GREY);
	VGA_writestring(" is the answer\n");

	KBD_initialize();

	while (1) {
		while (!KBD_poll());
		if (KBD_getevent(&evt) == 0) {
			if (evt.type == KEY_PRESSED && (c = KBD_getchar(&evt)))
				VGA_putchar(c);
		}
		KBD_flush();
	}
}
