// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.h
 *
 * Global descriptor table header file
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/11/18 - mrxx0 <chcoutur@student.42.fr>
 */

#ifndef GDT_H
#define GDT_H

/* Initialize the gdt and load it. */
void gdt_init();

/* Print the stack in a human friendly way. */
void print_stack();
/* Print the GDT info. */
void print_gdt();

#endif
