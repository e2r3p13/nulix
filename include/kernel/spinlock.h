// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* spinlock.h
 *
 * Header of the spinlock module: kernel/spinlock.c
 *
 * created: 2022/10/18 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef SPINLOCK_H
#define SPINLOCK_H

struct kspin {
	int lock;
	void *data;
};

void kspin_new(struct kspin *ks, void *data);
void *kspin_lock(struct kspin *ks);
int kspin_trylock(struct kspin *ks);
void kspin_drop(struct kspin *ks);

#endif
