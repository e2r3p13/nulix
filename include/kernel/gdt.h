// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* gdt.h
 *
 * Global descriptor table header file
 *
 * created: 2022/10/18 - mrxx0 <chcoutur@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef GDT_H
#define GDT_H

#define GDT_BASE 0x800

/* Initialize the gdt and load it. */
void gdt_init();

#endif
