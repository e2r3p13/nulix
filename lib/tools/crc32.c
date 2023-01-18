// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/tools/crc32.c
 *
 * Cyclic Redundancy Check 32
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <stddef.h>
#include <kernel/crc32.h>

#define CRC32_POLY 0x04C11DB7
#define CRC32_WIDTH 32
#define CRC32_TOPBIT 0x80000000

uint32_t crc32_get(uint8_t *data, size_t len) {
	uint32_t crc = 0xffffffff;
	for (size_t i = 0; i < len; i++) {
		crc ^= (uint32_t)(data[i]);
		for (size_t j = 0; j < 8; j++) {
			uint32_t mask = (0xffffffff - (crc & 1));
			if (mask == 0xffffffff)
				mask = 0;
			else
				mask++;
			crc = (crc >> 1) ^ (CRC32_POLY & mask);
		}
	}
	return 0xffffffff - crc;
}
