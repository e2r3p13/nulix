// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/spinlock.c
 *
 * Light spinlock library implementation
 *
 * created: 2022/10/18 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/spinlock.h>

/*
 * acquire_or_one aquires the spinlock if it's not
 * yet owned, in an atomic fashion
 * 
 * This allows to implement the locking mechanisms below
 *
 * Returns the old value of ks->lock, so:
 * 0 if the lock has been acqired
 * 1 if the lock is busy
 *
 */
static inline int acquire_or_one(struct kspin *ks) {
	return __sync_val_compare_and_swap(&ks->lock, 0, 1);
}

/*
 * To be caled to initialize a spinlock before any use
 * No particular locking mechanism involved here
 *
 */
void kspin_init(struct kspin *ks, void *data) {
	ks->lock = 0;
	ks->data = data;
}

/*
 * Polls the spinlock until the lock is aquered
 * similar to the pthread pthread_spinlock_lock function
 *
 * Returns the locked data when the lock is acquired
 */
void *kspin_lock(struct kspin *ks) {
	while (acquire_or_one(ks) == 1);
	return ks->data;
}

/* Tries to lock the spinlock, and returns immediately
 * similar to the pthread pthread_trylock function
 *
 * Returns 0 if the lock has been aquered, -1 otherwise
 */
int kspin_trylock(struct kspin *ks) {
	return -acquire_or_one(ks);
}

/*
 * Releases the spinlock
 * No locking mechanism involved here
 *
 */
void kspin_drop(struct kspin *ks) {
	ks->lock = 0;
}

