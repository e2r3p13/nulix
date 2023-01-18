// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/chacha.h
 *
 * Chacha header
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#ifndef CHACHA_H
#define CHACHA_H

#include <stdint.h>

typedef struct {
	uint32_t state[16];
} chacha_ctx;

void chacha_keysetup(chacha_ctx *x, const uint8_t *k);
void chacha_ivsetup(chacha_ctx *x, const uint8_t *iv);
int chacha_keystream_bytes(chacha_ctx *x, uint8_t *c, uint32_t bytes);

#endif
