// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* string.h
 *
 * Header file for the string library
 *
 * created: 2022/10/12 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef MATH_H
#define MATH_H

/*
	The math library defines some basic math operations.
	They are almost all named after the glibc ones, thus you can see their
	respective manual from your host.
*/

int sqrt(int n);
int pow(int base, int exp);
int log2(int n);
int log10(int n);

#endif
