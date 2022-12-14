// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/time.c
 *
 *
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/time.h>
#include <kernel/pit.h>

void sleep(uint32_t ms) {
	uint32_t time = PIT_TIMEMS + ms;
	while (PIT_TIMEMS < time);
}

uint32_t gettime() {
	return PIT_TIMEMS;
}
