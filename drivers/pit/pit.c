// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* drivers/pit/pit.c
 *
 *
 *
 * created: 2022/12/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/14 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <kernel/pit.h>
#include <kernel/port.h>

#define PIT_INTERNAL_FREQ 1193180

#define PIT_DATA_PORT 0x40
#define PIT_CMD_PORT 0x43

uint32_t pit_freq = 20; // Hz
uint32_t pit_counter = 0;

void pit_init() {
	pit_set_freq(pit_freq);
}

void pit_set_freq(uint32_t freq) {
	uint16_t div = PIT_INTERNAL_FREQ / freq;
	pit_freq = PIT_INTERNAL_FREQ / div;
	port_write_u8(PIT_CMD_PORT, 0x36);
	port_write_u8(PIT_DATA_PORT, div & 0xff);
	port_write_u8(PIT_DATA_PORT, div >> 8);
}
