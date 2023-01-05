// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/bit/bitmap.c
 *
 * Bitmap struct functions
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/05 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/bitmap.h>
#include <kernel/print.h>
#include <kernel/screenbuf.h>

extern struct screenbuf *sb_current;

int bitmap_init(struct bitmap *bitmap, size_t len, uint8_t *array, size_t size) {
	if (len / 8 > size || len == 0)
		return -1;
	bitmap->len = len;
	bitmap->size = size;
	bitmap->array = array;
	for (size_t i = 0; i < size; i++)
		bitmap->array[i] = 0;
	return 0;
}

int bitmap_set_all(struct bitmap *bitmap, int value) {
	int i;

	for (i = 0; (size_t)i < ((bitmap->len - 1) / 8) - 1; i++) {
		if (value)
			bitmap->array[i] = 0xff;
		else
			bitmap->array[i] = 0;
	}

	if (value)
		bitmap->array[i] = (~((size_t)(~0) << (bitmap->len % 8)));
	else
		bitmap->array[i] = 0;
	return 0;
}

int bitmap_set_at(struct bitmap *bitmap, size_t index, int value) {
	size_t array_index = index / 8;
	size_t bit_index = index % 8;

	if (array_index > (bitmap->len - 1) / 8)
		return -1;
	else if (array_index == (bitmap->len / 8) && bit_index >= bitmap->len % 8)
		return -1;
	if (value)
		bitmap->array[array_index] |= (1 << bit_index);
	else
		bitmap->array[array_index] &= ~(1 << bit_index);
	return 0;
}

int bitmap_get_at(struct bitmap *bitmap, size_t index) {
	size_t array_index = index / 8;
	size_t bit_index = index % 8;

	if (array_index > (bitmap->len - 1) / 8)
		return -1;
	else if (array_index == (bitmap->len / 8) && bit_index >= bitmap->len % 8)
		return -1;
	return (bitmap->array[array_index] & (1 << bit_index)) != 0;
}

int bitmap_get_first_one(struct bitmap *bitmap) {
	for (int i = 0; (size_t)i < ((bitmap->len - 1) / 8); i++) {
		uint8_t byte = bitmap->array[i];
		if (byte)
			return __builtin_ctz(byte) + (i * 8);
	}
	return -1;
}

int bitmap_get_last_one(struct bitmap *bitmap) {
	for (int i = (int)((bitmap->len - 1) / 8); i >= 0; i--) {
		uint8_t byte = bitmap->array[i];
		if (byte)
			return ((__builtin_clz(byte) + 1) - 8) + (i * 8);
	}
	return -1;
}

int bitmap_get_first_zero(struct bitmap *bitmap) {
	uint8_t byte;
	int i;
	for (i = 0; (size_t)i < ((bitmap->len - 1) / 8) - 1; i++) {
		byte = bitmap->array[i];
		byte = (~byte);
		if (byte)
			return __builtin_ctz(byte) + (i * 8);
	}
	byte = bitmap->array[i];
	byte = (~byte) & (~((size_t)(~0) << bitmap->len % 8));
	if (byte)
		return __builtin_ctz(byte) + (i * 8);
	return -1;
}

int bitmap_get_last_zero(struct bitmap *bitmap) {
	uint8_t byte;
	int i = (int)(bitmap->len - 1) / 8;
	byte = bitmap->array[i];
	byte = (~byte) & (~((size_t)(~0) << bitmap->len % 8));
	if (byte)
		return ((__builtin_clz(byte) + 1) - 8) + (i * 8);
	i--;
	for (; i >= 0; i--) {
		byte = bitmap->array[i];
		byte = (~byte);
		if (byte)
			return ((__builtin_clz(byte) + 1) - 8) + (i * 8);
	}
	return -1;
}
