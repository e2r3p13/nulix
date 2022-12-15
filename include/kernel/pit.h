// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/pit.h
 *
 * Driver for pit (Programmable Interval Timer)
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/15 - glafond- <glafond-@student.42.fr>
 */

#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#define PIT_TIMEMS (pit_counter_div * 1000 + (pit_counter_mod * 1000) / pit_freq)
#define PIT_REALTIMEMS (PIT_TIMEMS + ((PIT_TIMEMS/1000000) * pit_freq_mod))

extern volatile uint32_t pit_freq;
extern volatile uint32_t pit_freq_mod;

extern volatile uint32_t pit_counter_div;
extern volatile uint32_t pit_counter_mod;

void pit_init();

#endif
