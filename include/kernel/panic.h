// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/panic.h
 *
 * Insert file description here
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#ifndef PANIC_H
#define PANIC_H

#define PANIC \
do { \
	struct registers __regs = {0}; \
	asm("pushl $."); \
	asm("cli"); \
	REG_SAVE(&__regs); \
	panic_print(); \
	registers_print(&__regs); \
	asm("hlt"); \
} while (0)

#define PANIC_INTERRUPT(regs) \
do { \
	panic_print(); \
	registers_print((regs)); \
	asm("hlt"); \
} while (0)

void panic_print();

#endif
