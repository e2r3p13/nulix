// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/print_stack.c
 *
 * Helper function to print the kernel stack
 *
 * created: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/21 - mrxx0 <chcoutur@student.42.fr>
 */

#include <kernel/memory.h>

extern void *stack_bottom;
extern void *stack_top;

void print_stack() {
	hexdump(&stack_bottom, (void *)&stack_top - (void *)&stack_bottom);
}
