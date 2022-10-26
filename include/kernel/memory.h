// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* memory.h
 *
 * Header file for the kernel memory helpers
 *
 * created: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void hexdump(uint32_t *base, uint32_t limit);

#endif
