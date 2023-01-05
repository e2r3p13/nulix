// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/bitmaptree.h
 *
 * Bitmap tree header
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/05 - xlmod <glafond-@student.42.fr>
 */

#ifndef BITMAPTREE_H
#define BITMAPTREE_H

#include <stddef.h>
#include <stdint.h>

#include <kernel/bitmap.h>

struct bitmaptree {
	size_t height;
	size_t nleafs;
	struct bitmap *layers;
	size_t size;
};

int bitmaptree_init(struct bitmaptree *bmt, size_t nleafs, struct bitmap *layers,
					size_t height, uint8_t *array, size_t size);

int bitmaptree_get_index(struct bitmaptree *bmt, size_t size);

#endif
