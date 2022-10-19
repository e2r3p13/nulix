// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* print.h
 *
 * Header file for the print functions
 *
 * created: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef PRINT_H
#define PRINT_H

#include <stddef.h>

int snprintf(char *buf, size_t size, const char *fmt, ...);
int kprintf(const char *fmt, ...);

#endif
