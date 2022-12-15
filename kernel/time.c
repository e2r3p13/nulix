// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/time.c
 *
 * Time relative functions
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/time.h>
#include <kernel/pit.h>

/*
 * Sleep at least ms millisecond
 */
void sleep(uint32_t ms) {
	uint32_t time = PIT_REALTIMEMS + ms;
	while (PIT_REALTIMEMS < time);
}

/*
 * Return the time since the kernel start in millisecond
 */
uint32_t gettime() {
	return PIT_REALTIMEMS;
}
