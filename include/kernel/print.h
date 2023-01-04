// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* print.h
 *
 * Header file for the print functions
 *
 * created: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/04 - xlmod <glafond-@student.42.fr>
 */

#ifndef PRINT_H
#define PRINT_H

#include <stddef.h>

#define SERIAL_RESET  "\e[0m"
#define SERIAL_KDEBUG "\e[1;34m"
#define SERIAL_KINFO  "\e[1m"
#define SERIAL_KWARN  "\e[1;35m"
#define SERIAL_KERROR "\e[1;31m"

int snprintf(char *buf, size_t size, const char *fmt, ...);
int kprintf(const char *fmt, ...);
int klog(const char *fmt, ...);

#endif
