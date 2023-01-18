// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/crypto/chacha.c
 *
 * Chacha implementation
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/chacha.h>

static const unsigned int rounds = 20;

#define U8V(v) ((uint8_t)(v) & 0xFFU)
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFFU)

#define ROTL32(v, n) \
	(U32V((v) << (n)) | ((v) >> (32 - (n))))

#define U8TO32_LITTLE(p) \
	(((uint32_t)((p)[0])) | \
	 ((uint32_t)((p)[1]) << 8) | \
	 ((uint32_t)((p)[2]) << 16) | \
	 ((uint32_t)((p)[3]) << 24))

#define U32TO8_LITTLE(p, v) \
	do { \
		(p)[0] = U8V((v)); \
		(p)[1] = U8V((v) >> 8); \
		(p)[2] = U8V((v) >> 16); \
		(p)[3] = U8V((v) >> 24); \
	} while (0)

#define ROTATE(v, c) (ROTL32(v, c))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(v, w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

#define QUARTERROUND(a, b, c, d) \
	a = PLUS(a, b); d = ROTATE(XOR(d, a), 16); \
	c = PLUS(c, d); b = ROTATE(XOR(b, c), 12); \
	a = PLUS(a, b); d = ROTATE(XOR(d, a), 8); \
	c = PLUS(c, d); b = ROTATE(XOR(b, c), 7);

static const char sigma[16] = "expand 32-byte k";

void chacha_keysetup(chacha_ctx *x, const uint8_t *k) {
    x->state[0] = U8TO32_LITTLE(sigma + 0);
    x->state[1] = U8TO32_LITTLE(sigma + 4);
    x->state[2] = U8TO32_LITTLE(sigma + 8);
    x->state[3] = U8TO32_LITTLE(sigma + 12);
    x->state[4] = U8TO32_LITTLE(k + 0);
    x->state[5] = U8TO32_LITTLE(k + 4);
    x->state[6] = U8TO32_LITTLE(k + 8);
    x->state[7] = U8TO32_LITTLE(k + 12);
    x->state[8] = U8TO32_LITTLE(k + 16);
    x->state[9] = U8TO32_LITTLE(k + 20);
    x->state[10] = U8TO32_LITTLE(k + 24);
    x->state[11] = U8TO32_LITTLE(k + 28);
}

void chacha_ivsetup(chacha_ctx *x, const uint8_t *iv) {
    x->state[12] = 0;
    x->state[13] = 0;
    x->state[14] = U8TO32_LITTLE(iv + 0);
    x->state[15] = U8TO32_LITTLE(iv + 4);
}

int chacha_keystream_bytes(chacha_ctx *x, uint8_t *c, uint32_t bytes) {
	if (!bytes)
		return -1;

	uint8_t *ctarget;
	uint8_t tmp[64];

	uint32_t s[16];
	uint32_t k[16];

	for (int i = 0; i < 16; i++)
		s[i] = x->state[i];

	for (;;) {

		if (bytes < 64) {
			ctarget = c;
			c = tmp;
		}

		for (int i = 0; i < 16; i++)
			k[i] = s[i];

		for (int i = rounds; i > 0; i -= 2) {
			QUARTERROUND(k[0], k[4], k[8], k[12])
            QUARTERROUND(k[1], k[5], k[9], k[13])
            QUARTERROUND(k[2], k[6], k[10], k[14])
            QUARTERROUND(k[3], k[7], k[11], k[15])
            QUARTERROUND(k[0], k[5], k[10], k[15])
            QUARTERROUND(k[1], k[6], k[11], k[12])
            QUARTERROUND(k[2], k[7], k[8], k[13])
            QUARTERROUND(k[3], k[4], k[9], k[14])
		}

		for (int i = 0; i < 16; i++)
			k[i] = PLUS(k[i], s[i]);

		s[12] = PLUSONE(s[12]);
		if (!s[12])
			s[13] = PLUSONE(s[13]);

		for (int i = 0; i < 16; i++)
			U32TO8_LITTLE(c + (i * 4), k[i]);

		if (bytes <= 64) {
			if (bytes < 64) {
				for (uint32_t i = 0; i < bytes; ++i)
					ctarget[i] = c[i];
			}
			x->state[12] = s[12];
			x->state[13] = s[13];
			return 0;
		}
		bytes -= 64;
		c += 64;
	}
}
