// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/kmalloc.h
 *
 * Kmalloc header
 *
 * created: 2023/01/09 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>

#define KMF_NORMAL 0
#define KMF_NOFAIL 1

int kmalloc_init();

void *kmalloc(size_t size, int flag);
void kfree(void *addr);

void *krealloc(void *addr, size_t size, int flag);
void *kzalloc(size_t size, int flag);


#endif
