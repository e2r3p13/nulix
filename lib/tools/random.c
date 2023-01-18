// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/tools/random.c
 *
 * Random
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <stddef.h>
#include <kernel/random.h>
#include <kernel/rtc.h>
#include <kernel/chacha.h>

static chacha_ctx x;

void random_fill_array(uint8_t *a, size_t bytes) {
	uint32_t t = rtc_get_timestamp();
	uint32_t tmp = 0;
	for (size_t i = 0; i < bytes; i++) {
		tmp ^= t;
		a[i] = tmp;
		tmp <<= 8;
	}
}

void random_init() {
	uint8_t key[32];
	random_fill_array(key, 32);
	chacha_keysetup(&x, key);
}

int random_get(uint8_t *data, size_t bytes) {
	if (!bytes)
		return -1;
	uint8_t iv[8];
	random_fill_array(iv, 8);
	chacha_ivsetup(&x, iv);
	chacha_keystream_bytes(&x, data, bytes);
	uint32_t t = rtc_get_timestamp();
	for (size_t i = 0; i < bytes; i++)
		data[i] ^= ((t >> ((i % 4) * 8)) & 0xff);
	return 0;
}
