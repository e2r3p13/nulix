// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* drivers/pit/pit.c
 *
 * Driver for pit (Programmable Interval Timer)
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <kernel/pit.h>
#include <kernel/port.h>

#define PIT_INTERNAL_FREQ 1193180

#define PIT_DATA_PORT 0x40
#define PIT_CMD_PORT 0x43

#define PIT_TIMER_MODE_SQUARE 3
#define PIT_TIMER_RW_LSBMSB 3

#define PIT_SETTIMER_CMD(mode, rw) ((mode << 1) | (rw << 4))

volatile uint32_t pit_freq = 1193; // Hz
volatile uint32_t pit_freq_mod = 0;

volatile uint32_t pit_counter_div = 0;
volatile uint32_t pit_counter_mod = 0;

static inline void pit_set_freq(uint32_t freq) {
	uint16_t divisor = PIT_INTERNAL_FREQ / freq;
	pit_freq_mod = PIT_INTERNAL_FREQ % freq;
	pit_freq = freq;
	port_write_u8(PIT_CMD_PORT, PIT_SETTIMER_CMD(PIT_TIMER_MODE_SQUARE, PIT_TIMER_RW_LSBMSB));
	port_write_u8(PIT_DATA_PORT, divisor & 0xff);
	port_write_u8(PIT_DATA_PORT, divisor >> 8);
}

void pit_init() {
	pit_set_freq(1193);
}
