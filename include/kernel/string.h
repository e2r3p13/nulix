// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* string.h
 *
 * Header file for the string library
 *
 * created: 2022/10/12 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/*
	The string library defines some basic string/memory manipulation functions.
	They are almost all named after the glibc ones, thus you can see their
	respective manual from your host.
*/

/* lib/string/str.c */
size_t	strlen(const char *s);
char	*strchr(const char *s, int c);
char	*strrchr(const char *s, int c);
int		strcmp(const char *s1, const char *s2);
int		strncmp(const char *s1, const char *s2, size_t n);
char	*strcpy(char *dest, const char *src);
char	*strncpy(char *dest, const char *src, size_t n);
size_t	strspn(const char *s, const char *accept);
size_t	strcspn(const char *s, const char *reject);
char	*strpbrk(const char *s, const char *accept);
char	*strstr(const char *haystack, const char *needle);
char	*strtok(char *str, const char *delim);
char	*strcat(char *dst, const char *src);
char	*strncat(char *dst, const char * src, size_t n);

/* lib/string/mem.c */
void	*memchr(const void *s, int c, size_t n);
void	*memrchr(const void *s, int c, size_t n);
int		memcmp(const void *s1, const void *s2, size_t n);
void	*memcpy(void *dst, const void *src, size_t n);
void	*memmove(void *dst, const void *src, size_t n);
void	*memset(void *s, int c, size_t n);

#endif
