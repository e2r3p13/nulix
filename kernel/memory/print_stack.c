// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/memory/print_stack.c
 *
 * Helper function to print the kernel stack
 *
 * created: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/memory.h>

extern uint32_t *stack_bottom;

void print_stack() {
	uint32_t stack_size = 16384;
	hexdump(stack_bottom, stack_size);	
}
