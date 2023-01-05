// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/bit/bitmaptree.c
 *
 * Bitmap tree function handler
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/05 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/bitmaptree.h>

int bitmaptree_init(struct bitmaptree *bmt, size_t nroots, struct bitmap *layers,
					size_t height, uint8_t *array, size_t size) {
	if (!nroots || !height || !size)
		return -1;
	size_t csize = 0;
	for (size_t i = 0, n = nroots; i < height; i++, n *= 2)
		csize += BM_NBYTES_FROM_NBITS(n);
	if (size < csize)
		return -1;

	bmt->nroots = nroots;
	bmt->height = height;
	bmt->layers = layers;
	bmt->size = size;

	for (size_t i = 0, n = nroots; i < height; i++, n *= 2) {
		if (bitmap_init(&bmt->layers[i], n, array, BM_NBYTES_FROM_NBITS(n)))
			return -1;
		array += bmt->layers[i].size;
	}
	return 0;
}
