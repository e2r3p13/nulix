// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/bit/bitmaptree.c
 *
 * Bitmap tree function handler
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/06 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/bitmaptree.h>

int bitmaptree_init(struct bitmaptree *bmt, size_t nleafs, struct bitmap *layers,
					size_t height, uint8_t *array, size_t size) {
	if (!nleafs || !height || !size)
		return -1;
	if (!(nleafs >> (height - 1)))
		return -1;
	size_t csize = 0;
	for (size_t i = 0, n = nleafs; i < height; i++, n *= 2)
		csize += BM_NBYTES_FROM_NBITS(n);
	if (size < csize)
		return -1;

	bmt->nleafs = nleafs;
	bmt->height = height;
	bmt->layers = layers;
	bmt->size = size;

	for (size_t i = 0, n = nleafs; i < height; i++, n *= 2) {
		if (bitmap_init(&bmt->layers[i], n, array, BM_NBYTES_FROM_NBITS(n)))
			return -1;
		array += bmt->layers[i].size;
	}
	return 0;
}

int bitmaptree_get_fit(struct bitmaptree *bmt, size_t len) {
	if (!len)
		return -1;

	size_t l;
	for (l = bmt->height - 1; len < (size_t)(1 << l); l--);

	size_t ref_index = 0;
	for (;;) {
		int first_zero = bitmap_get_next_zero(&bmt->layers[l], ref_index);
		if (first_zero < 0)
			return -1;
		size_t start_index = first_zero;
		if (start_index) {
			for (int layer_index = (int)l; layer_index >= 0; layer_index--) {
				start_index = (start_index * 2) - 1;
				if (bitmap_get_at(&bmt->layers[layer_index], start_index))
					start_index++;
			}
		}
		// start_index == index sur l0 libre

		int last_zero = bitmap_get_next_one(&bmt->layers[l], first_zero);
		if (last_zero < 0)
			last_zero = bmt->layers[l].len;
		size_t end_index = last_zero;
		for (int layer_index = (int)l; layer_index >= 0; layer_index--) {
			end_index *= 2;
			if (bitmap_get_at(&bmt->layers[layer_index], end_index) == 0)
				end_index++;
		}
		// end_index == index sur l0 prit
		
		if (end_index - start_index >= len)
			return start_index;
		else
			ref_index = last_zero;
	}
}

int bitmaptree_set_from(struct bitmaptree *bmt, size_t index, size_t len, size_t value) {
	if (!len || index + len >= bmt->nleafs)
		return -1;

	bitmap_set_from(&bmt->layers[0], index, len, value);
	bitmaptree_update(bmt, index, len);
	return 0;
}

int bitmaptree_update(struct bitmaptree *bmt, size_t index, size_t len) {
	if (!len || index + len >= bmt->nleafs)
		return -1;

	for (size_t l = 1; l < bmt->height; l++) {
		for (size_t i = index, count = 0; count < (len + 1) / 2; count++, i /= 2) {
			int val = bitmap_get_at(&bmt->layers[l - 1], i)
					+ bitmap_get_at(&bmt->layers[l - 1], i + 1);
			bitmap_set_at(&bmt->layers[l / 2], i, val);
		}
	}
	return 0;
}
