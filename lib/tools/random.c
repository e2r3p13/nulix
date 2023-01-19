// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/tools/random.c
 *
 * Random
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/19 - xlmod <glafond-@student.42.fr>
 */

#include <stdint.h>
#include <stddef.h>
#include <kernel/random.h>
#include <kernel/rtc.h>
#include <kernel/chacha.h>
#include <kernel/string.h>

#define RND_NITER 48
#define RND_BLOCKSIZE 64
#define RND_BYTESBUFFER ((RND_NITER - 3) * RND_BLOCKSIZE)

static chacha_ctx x;
static uint8_t input[RND_BLOCKSIZE] = {0};
static uint8_t buffer[RND_NITER][RND_BLOCKSIZE] = {{0}};
static uint8_t spare[RND_BLOCKSIZE / 2] = {0};

/*
 * Fill @out with @bytes random bytes.
 *
 * NOTE: took inspiration of the Daniel J. Bernstein blog post
 * https://blog.cr.yp.to/20170723-random.html
 */
static void random_generate_buffer(uint8_t *out, size_t bytes) {
	uint8_t tmp[RND_BLOCKSIZE] = {0};
	uint8_t nonce[8] = {0};

	for (int i = 0; i < RND_NITER; i++) {

		nonce[0]++;
		chacha_ivsetup(&x, (uint8_t *)nonce);

		chacha_keystream_bytes(&x, tmp, RND_BLOCKSIZE);
		for (int j = 0; j < RND_BLOCKSIZE; j++) {
			input[j] = input[j] ^ tmp[j];
			buffer[i][j] = input[j];
			tmp[j] = 0;
		}

		// XOR the key with the spare
		if (i == RND_NITER / 2) {
			uint8_t *ptr = (uint8_t *)(x.state + 4);
			for (int j = 0; j < RND_BLOCKSIZE / 2; j++) {
				ptr[j] = ptr[j] ^ spare[j];
				spare[j] = 0;
			}
		}
	}

	// Replace the key by the first 256bit of b[0] ^ b[1]
	// Replace the spare by the last 256bit of b[0] ^ b[1]
	// Replace the input by b[47]
	// Erase each bytes as soon as it's consumed
	uint8_t *ptr = (uint8_t *)(x.state + 4);
	for (int j = 0, k = RND_BLOCKSIZE / 2; k < RND_BLOCKSIZE; j++, k++) {
		ptr[j] = buffer[0][j] ^ buffer[1][j];
		spare[j] = buffer[0][k] ^ buffer[1][k];
		buffer[0][j] = 0;
		buffer[0][k] = 0;
		buffer[1][j] = 0;
		buffer[1][k] = 0;
		input[j] = buffer[RND_NITER - 1][j];
		input[k] = buffer[RND_NITER - 1][k];
		buffer[RND_NITER - 1][j] = 0;
		buffer[RND_NITER - 1][k] = 0;
	}

	// Copy the buffer in the output
	for (int i = 2; i < RND_NITER - 1; i++) {
		uint8_t tmp = bytes < RND_BLOCKSIZE ? bytes : RND_BLOCKSIZE;
		memcpy(out, buffer[i], tmp);
		memset(buffer[i], 0, RND_BLOCKSIZE);
		bytes -= tmp;
		out += tmp;
	}
}

void random_init() {
	uint8_t key[RND_BLOCKSIZE / 2];
	uint32_t t = rtc_get_timestamp();
	uint8_t t1, t2;
	t1 = ((t >> 24) & 0xff);
	t1 ^= ((t) & 0xff);
	t2 = ((t >> 16) & 0xff);
	t2 ^= ((t) & 0xff);

	for (int j = 0; j < RND_BLOCKSIZE / 2; j++) {
		key[j] = ((t1 ^ j) + (t1 >> (j % 8))) % 0xff;
		spare[j] = ((t2 ^ j) + (t2 >> (j % 8))) % 0xff;
		input[(j * 2)] = t1;
		input[(j * 2) + 1] = t2;
	}
	chacha_keysetup(&x, key);
	memset(key, 0, RND_BLOCKSIZE / 2);
}

int random_get(uint8_t *data, size_t bytes) {
	if (!bytes)
		return -1;
	for (;;) {
		random_generate_buffer(data, bytes);
		if (bytes <= RND_BYTESBUFFER)
			return 0;
		data += RND_BYTESBUFFER;
		bytes -= RND_BYTESBUFFER;
	}
}
