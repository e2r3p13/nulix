// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/bitmaptree.h
 *
 * Bitmap tree header
 *
 * created: 2023/01/05 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/09 - glafond- <glafond-@student.42.fr>
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

int bitmaptree_alloc(struct bitmaptree *bmt, size_t nleafs, size_t height, int type);

int bitmaptree_init(struct bitmaptree *bmt, size_t nleafs, struct bitmap *layers,
					size_t height, uint8_t *array, size_t size);

int bitmaptree_get_fit(struct bitmaptree *bmt, size_t len);
int bitmaptree_set_from(struct bitmaptree *bmt, size_t index, size_t len, size_t value);
int bitmaptree_set_from_if(struct bitmaptree *bmt, struct bitmap *control,
					size_t index, size_t len, size_t value);

int bitmaptree_update(struct bitmaptree *bmt, size_t index, size_t len);

#endif
