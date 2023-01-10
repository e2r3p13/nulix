// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* spinlock.h
 *
 * Header of the spinlock module: kernel/spinlock.c
 *
 * created: 2022/10/18 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
 */

#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef int spinlock_t;

void kspin_init(spinlock_t *lock);
void kspin_lock(spinlock_t *lock);
int kspin_trylock(spinlock_t *lock);
void kspin_drop(spinlock_t *lock);

#endif
