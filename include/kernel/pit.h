// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/pit.h
 *
 *
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - xlmod <glafond-@student.42.fr>
 */

#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#define PIT_TIMEMS ((pit_counter * 1000) / pit_freq)

extern uint32_t pit_freq;
extern uint32_t pit_counter;

void pit_init();
void pit_set_freq(uint32_t freq);

#endif
