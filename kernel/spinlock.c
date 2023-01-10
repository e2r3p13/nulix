// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/spinlock.c
 *
 * Light spinlock library implementation
 *
 * created: 2022/10/18 - lfalkau <lfalkau@student.42.fr>
 * updated: 2023/01/10 - glafond- <glafond-@student.42.fr>
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
static inline int acquire_or_one(spinlock_t *lock) {
	return __sync_val_compare_and_swap(lock, 0, 1);
}

/*
 * To be caled to initialize a spinlock before any use
 * No particular locking mechanism involved here
 *
 */
void kspin_init(spinlock_t *lock) {
	*lock = 0;
}

/*
 * Polls the spinlock until the lock is aquered
 * similar to the pthread pthread_spinlock_lock function
 *
 * Returns the locked data when the lock is acquired
 */
void kspin_lock(spinlock_t *lock) {
	while (acquire_or_one(lock) == 1);
}

/* Tries to lock the spinlock, and returns immediately
 * similar to the pthread pthread_trylock function
 *
 * Returns 0 if the lock has been aquered, -1 otherwise
 */
int kspin_trylock(spinlock_t *lock) {
	return -acquire_or_one(lock);
}

/*
 * Releases the spinlock
 * No locking mechanism involved here
 *
 */
void kspin_drop(spinlock_t *lock) {
	*lock = 0;
}

