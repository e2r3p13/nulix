// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* panic.c
 *
 * Panic screen handler
 *
 * created: 2022/12/19 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2023/01/25 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/print.h>

void panic_print() {
	kprintf("\n");
	kprintf("###############################################################################\n");
	kprintf("#                        ___  ___  _  _  ___   ___                            #\n");
	kprintf("#                       | _ \\/   \\| \\| ||_ _| / __|                           #\n");
	kprintf("#                       |  _/| - || .  | | | | (__                            #\n");
	kprintf("#                       |_|  |_|_||_|\\_||___| \\___|                           #\n");
	kprintf("#                                                                             #\n");
	kprintf("###############################################################################\n");
	kprintf("\n");
}
