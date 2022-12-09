// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* lib/std/abs.c
 *
 * Little absolute functions
 *
 * created: 2022/12/08 - xlmod <glafond-@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

/*
 * Return the absolute value of an interger.
 */

int abs(int x) {
	return x < 0 ? -x : x;
}

long labs(long x) {
	return x < 0 ? -x : x;
}

long long llabs(long long x) {
	return x < 0 ? -x : x;
}
