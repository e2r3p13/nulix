// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* mem.c
 *
 * memory related functions of the string library
 *
 * created: 2022/10/12 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <stddef.h>

void *memchr(const void *s, int c, size_t n) {
	unsigned char *sc = (unsigned char *)s;

	while (n--) {
		if (*sc == c)
			return (void *)sc;
		sc++;
	}
	return NULL;
}

void *memrchr(const void *s, int c, size_t n) {
	unsigned char *sc = (unsigned char *)s;

	sc += n;
	while (n--) {
		if (*sc == c)
			return (void *)sc;
		sc--;
	}
	return NULL;
}

int  memcmp(const void *s1, const void *s2, size_t n) {
	unsigned char *sc1 = (unsigned char *)s1;
	unsigned char *sc2 = (unsigned char *)s2;

	while (n > 0 && *sc1 == *sc2) {
		sc1++, sc2++, n--;
	}
	return n == 0 ? 0 : *sc1 - *sc2;
}

void *memcpy(void *dst, const void *src, size_t n) {
	void *ptr = dst;

	while (n--) {
		*(unsigned char *)dst = *(unsigned char *)src;
		dst++, src++;
	}
	return ptr;
}

void *memmove(void *dst, const void *src, size_t n) {
	unsigned char *dstc = dst;
	const unsigned char *srcc = src;
	void *ptr = dst;

	while (n--) {
		if (dstc > srcc) {
			*(dstc + n) = *(srcc + n);
		} else {
			*dstc = *srcc;
			dstc++, srcc++;
		}
	}
	return ptr;
}

void *memset(void *s, int c, size_t n) {
	unsigned char *sc = s;
	void *ptr = s;

	while (n--) {
		*sc++ = c;
	}
	return ptr;
}
