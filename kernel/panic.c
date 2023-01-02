// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* panic.c
 *
 * Panic screen handler
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/02 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/panic.h>

extern struct screenbuf *sb_current;

static void print_panic_frame(t_panic_frame *panic_frame) {
	kprintf("eax: %8p ", panic_frame->eax);
	kprintf("ebx: %8p ", panic_frame->ebx);
	kprintf("ecx: %8p ", panic_frame->ecx);
	kprintf("edx: %8p\n", panic_frame->edx);
	kprintf("esi: %8p ", panic_frame->esi);
	kprintf("edi: %8p ", panic_frame->edi);
	kprintf("ebp: %8p ", panic_frame->ebp);
	kprintf("esp: %8p\n", panic_frame->esp);
	kprintf("eip: %8p ", panic_frame->eip);
	kprintf("efl: %8p\n", panic_frame->eflags);
	kprintf("es: %4p\n", panic_frame->es);
	kprintf("cs: %4p\n", panic_frame->cs);
	kprintf("ss: %4p\n", panic_frame->ss);
	kprintf("ds: %4p\n", panic_frame->ds);
	kprintf("fs: %4p\n", panic_frame->fs);
	kprintf("gs: %4p\n", panic_frame->gs);
	kprintf("cr0: %8p ", panic_frame->cr0);
	kprintf("cr2: %8p ", panic_frame->cr2);
	kprintf("cr3: %8p ", panic_frame->cr3);
}

void panic() {
sb_clear(sb_current);
t_panic_frame panic_frame = {0};
LOAD_PANIC_STACK;
asm( "mov %%eax, %0" : "=r" ( panic_frame.eax ));
asm( "mov %%ecx, %0" : "=rm" ( panic_frame.ecx ));
asm( "mov %%edx, %0" : "=rm" ( panic_frame.edx ));
asm( "mov %%ebx, %0" : "=rm" ( panic_frame.ebx ));
asm( "mov %%esp, %0" : "=rm" ( panic_frame.esp ));
asm( "mov %%ebp, %0" : "=rm" ( panic_frame.ebp ));
asm( "mov %%esi, %0" : "=rm" ( panic_frame.esi ));
asm( "mov %%edi, %0" : "=rm" ( panic_frame.edi ));
asm( "1: movl $1b, %0" : "=rm" (panic_frame.eip));
asm("pushf\n\t" "pop %0" : "=rm" (panic_frame.eflags ));
asm( "mov %%cs, %0" : "=rm" ( panic_frame.cs ));
asm( "mov %%ds, %0" : "=rm" ( panic_frame.ds ));
asm( "mov %%es, %0" : "=rm" ( panic_frame.es ));
asm( "mov %%fs, %0" : "=rm" ( panic_frame.fs ));
asm( "mov %%gs, %0" : "=rm" ( panic_frame.gs ));
asm( "mov %%ss, %0" : "=rm" ( panic_frame.ss ));
asm( "mov %%cr0, %0" : "=rm" ( panic_frame.cr0 ));
asm( "mov %%cr3, %0" : "=rm" ( panic_frame.cr3 ));
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
kprintf("--\n");
print_panic_frame(&panic_frame);
__asm__ volatile("hlt");
}
