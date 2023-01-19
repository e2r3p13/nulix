// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/random.h
 *
 * Random
 *
 * created: 2023/01/18 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/19 - xlmod <glafond-@student.42.fr>
 */

#ifndef RANDOM_H
#define RANDOM_H

void random_init();
int random_get(uint8_t *data, size_t len);

#endif
