// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/bit/bitmap.c
 *
 * Bitmap struct functions
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/11 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/bitmap.h>
#include <kernel/print.h>
#include <kernel/kmalloc.h>

int bitmap_alloc(struct bitmap *bitmap, size_t len, int flag) {
	if (!len)
		return -1;
	size_t size = ((len - 1) / 8) + 1;
	void *array = kmalloc(size * sizeof(uint8_t), flag);
	if (!array)
		return -1;
	if (bitmap_init(bitmap, len, array, size) < 0) {
		kfree(array);
		return -1;
	}
	return 0;
}

int bitmap_init(struct bitmap *bitmap, size_t len, uint8_t *array, size_t size) {
	if (!len || !size || len / 8 > size)
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

	if (index >= bitmap->len)
		return -1;
	if (value)
		bitmap->array[array_index] |= (1 << bit_index);
	else
		bitmap->array[array_index] &= ~(1 << bit_index);
	return 0;
}

int bitmap_set_from(struct bitmap *bitmap, size_t index, size_t len, int value) {
	size_t array_index = index / 8;
	size_t bit_index = index % 8;
	size_t nset = 0;

	if (!len || index + len > bitmap->len)
		return -1;
	for (size_t ai = array_index; len > 0; ai++) {
		for (size_t bi = bit_index; bi < 8 && len > 0; bi++, len--) {
			if ((bitmap_get_at(bitmap, (ai * 8) + bi) ^ (value && 1))) {
				nset++;
			}
			if (value)
				bitmap->array[ai] |= (1 << bi);
			else
				bitmap->array[ai] &= ~(1 << bi);
		}
		bit_index = 0;
	}
	return nset;
}

int bitmap_set_from_if(struct bitmap *bitmap, struct bitmap *control, size_t index, size_t len, int value) {
	size_t array_index = index / 8;
	size_t bit_index = index % 8;
	size_t nset = 0;

	if (!len || index + len > bitmap->len)
		return -1;
	if (control->len != bitmap->len)
		return -1;
	for (size_t ai = array_index; len > 0; ai++) {
		for (size_t bi = bit_index; bi < 8 && len > 0; bi++, len--) {
			if (bitmap_get_at(control, (ai * 8) + bi) == 1) {
				if ((bitmap_get_at(bitmap, (ai * 8) + bi) ^ (value && 1))) 
					nset++;
				if (value)
					bitmap->array[ai] |= (1 << bi);
				else
					bitmap->array[ai] &= ~(1 << bi);
			}
		}
		bit_index = 0;
	}
	return nset;
}

int bitmap_get_at(struct bitmap *bitmap, size_t index) {
	size_t array_index = index / 8;
	size_t bit_index = index % 8;

	if (index >= bitmap->len)
		return -1;
	return (bitmap->array[array_index] & (1 << bit_index)) != 0;
}

int bitmap_get_next_one(struct bitmap *bitmap, size_t index) {
	if (index >= bitmap->len)
		return -1;
	int i = (int)(index / 8);
	uint8_t byte = bitmap->array[i];
	byte &= ((size_t)(~0U) << (index % 8));
	if (byte)
		return __builtin_ctz(byte) + (i * 8);
	i++;
	for (; (size_t)i < ((bitmap->len - 1) / 8); i++) {
		byte = bitmap->array[i];
		if (byte)
			return __builtin_ctz(byte) + (i * 8);
	}
	return -1;
}

int bitmap_get_prev_one(struct bitmap *bitmap, size_t index) {
	if (index >= bitmap->len)
		return -1;
	int i = (int)(index / 8);
	uint8_t byte = bitmap->array[i];
	byte &= (~((size_t)(~0) << (index % 8)));
	if (byte)
		return ((__builtin_clz(byte) + 1) - 8) + (i * 8);
	i--;
	for (; i >= 0; i--) {
		uint8_t byte = bitmap->array[i];
		if (byte)
			return ((__builtin_clz(byte) + 1) - 8) + (i * 8);
	}
	return -1;
}

int bitmap_get_first_one(struct bitmap *bitmap) {
	return bitmap_get_next_one(bitmap, 0);
}

int bitmap_get_last_one(struct bitmap *bitmap) {
	return bitmap_get_prev_one(bitmap, bitmap->len - 1);
}

int bitmap_get_next_zero(struct bitmap *bitmap, size_t index) {
	if (index >= bitmap->len)
		return -1;
	int i = (int)(index / 8);
	uint8_t byte = bitmap->array[i];
	byte = (~byte) & ((size_t)(~0) << (index % 8));
	if (byte)
		return __builtin_ctz(byte) + (i * 8);
	i++;
	for (; (size_t)i < ((bitmap->len - 1) / 8) - 1; i++) {
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

int bitmap_get_prev_zero(struct bitmap *bitmap, size_t index) {
	if (index >= bitmap->len)
		return -1;
	int i = (int)(index / 8);
	uint8_t byte = bitmap->array[i];
	byte = (~byte) & (~((size_t)(~0) << (index % 8)));
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

int bitmap_get_first_zero(struct bitmap *bitmap) {
	return bitmap_get_next_zero(bitmap, 0);
}

int bitmap_get_last_zero(struct bitmap *bitmap) {
	return bitmap_get_prev_zero(bitmap, bitmap->len - 1);
}

