// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/kmalloc_eternal.h
 *
 * Kmalloc eternal header
 *
 * created: 2023/01/10 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#ifndef KMALLOC_ETERNAL_H
#define KMALLOC_ETERNAL_H

struct kmalloc_eternal {
	virtaddr_t start;
	virtaddr_t end;
	virtaddr_t cur;
	struct kpm_chunk pchunk;
};

int kmalloc_eternal_init();
void *kmalloc_eternal(size_t size);

#endif
