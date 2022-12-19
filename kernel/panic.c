// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* panic.c
 *
 * Panic screen handler
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/panic.h>

extern struct screenbuf *sb_current;

static void print_panic_frame(t_panic_frame *panic_frame) {
	kprintf("frame interupt: %p\n",panic_frame);
	kprintf("eax: %x\t", panic_frame->eax);
	kprintf("ebx: %x\t", panic_frame->ebx);
	kprintf("ecx: %x\t", panic_frame->ecx);
	kprintf("edx: %x\n", panic_frame->ecx);
//	kprintf("cs: %x\t", panic_frame->cs);
//	kprintf("ds: %x\t", panic_frame->ds);
//	kprintf("es: %x\t", panic_frame->es);
//	kprintf("fs: %x\t", panic_frame->fs);
}

void panic() {
LOAD_PANIC_STACK;
t_panic_frame *panic_frame = {0};
kprintf("/\\\n");
kprintf("||_____-----_____-----_____\n");
kprintf("||   O                  O  \\\n");
kprintf("||    O\\    ____   //O    /\n");
kprintf("||       \\ /     \\//      \\\n");
kprintf("||         |_O O_|          /\n");
kprintf("||          ^ | ^           \\\n");
kprintf("||        // UUU \\         /\n");
kprintf("||    O//   PANIC \\O      \\\n");
kprintf("||   O               O     /\n");
kprintf("||_____-----_____-----_____\\\n");
kprintf("||\n");
kprintf("||\n");
kprintf("||\n");
kprintf("||\n");
kprintf("||\n");
kprintf("--");

print_panic_frame(panic_frame);

}
