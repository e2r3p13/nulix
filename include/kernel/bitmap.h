// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/bitmap.h
 *
 * Bitmap struct header
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <stddef.h>
#include <stdint.h>

#define ALIGNNEXT(size, align)		((((uint32_t)(size)) + ((align) - 1)) & (0xffffffff ^ ((align) - 1)))
#define BM_NBYTES_FROM_NBITS(n)		(ALIGNNEXT(n, 8) / 8)

struct bitmap {
	size_t	len;
	uint8_t	*array;	
	size_t	size;
};

int bitmap_alloc(struct bitmap *bitmap, size_t len, int type);

int bitmap_init(struct bitmap *bitmap, size_t len, uint8_t *array, size_t size);

int bitmap_set_all(struct bitmap *bitmap, int value);

int bitmap_set_at(struct bitmap *bitmap, size_t index, int value);
int bitmap_get_at(struct bitmap *bitmap, size_t index);

int bitmap_set_from(struct bitmap *bitmap, size_t index, size_t len, int value);
int bitmap_set_from_if(struct bitmap *bitmap, struct bitmap *control,
				size_t index, size_t len, int value);

int bitmap_get_next_one(struct bitmap *bitmap, size_t index);
int bitmap_get_prev_one(struct bitmap *bitmap, size_t index);
int bitmap_get_first_one(struct bitmap *bitmap);
int bitmap_get_last_one(struct bitmap *bitmap);

int bitmap_get_next_zero(struct bitmap *bitmap, size_t index);
int bitmap_get_prev_zero(struct bitmap *bitmap, size_t index);
int bitmap_get_first_zero(struct bitmap *bitmap);
int bitmap_get_last_zero(struct bitmap *bitmap);

#endif
