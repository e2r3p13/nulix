// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* print.c
 *
 * Implements some functions to print format strings
 *
 * created: 2022/10/15 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/09 - xlmod <glafond-@student.42.fr>
 */

#include <stdarg.h>
#include <kernel/string.h>
#include <kernel/screenbuf.h>

#define BUFSIZE 4096

extern struct screenbuf sb[];
extern int sb_index;

/*
 * Computes the pow of a number in a specified base
 *
 * @n: The number
 * @base: The base
 *
 * ret: The pow of @n in base @base, which is also the length of @n once written
 */
static int basepow(unsigned int n, int base) {
	int len = 0;

	do {
		n /= base;
		len++;
	} while (n != 0);

	return len;
}

/*
 * Writes at most @size bytes of the number @n in base @base into the buffer @buf
 *
 * @buf: The buffer to write in
 * @size: The maximum amount of bytes to write
 * @n: The number to write
 * @base: The base in which to write @n
 * @issigned: Weither or not the number can be negative. If it's set to
 *            false and @n is negative, it will be casted to an unsigned int
 *
 * @ret: Number of written bytes
 */
static int snprintnum(char *buf, size_t size, int n, int base, int issigned, int padding) {
	const char digits[] = "0123456789abcdef";
	unsigned int un;
	size_t count = 0;
	int nl;

	if (base > 16 || size == 0)
		return 0;

	if (issigned && n < 0) {
		buf++[count++] = '-';
		un = -n;
	} else {
		un = n;
	}

	nl = basepow(un, base);
	padding -= nl;

	for (int i = 0; i < padding; i++) {
		*buf++ = '0';
		count++;
		if (count == size)
			break;
	}

	do {
		nl--;
		if (count + nl < size) {
			buf[nl] = digits[un % base];
			count++;
		}
		un /= base;
	} while (un != 0);
	return count;
}

/*
 * Writes at most @size bytes of the format string @fmt, into @buf,
 * expending specifiers: see man printf
 * 
 * It behaves (almost) as the real printf implementation, but only handles the
 * following specifiers, without any flags, mor length or precision:
 * - %%: percent
 * - %c: character
 * - %s: string
 * - %d: signed integer
 * - %u: unsigned integer
 * - %b: unsigned binary integer
 * - %x: unsigned hexadecimal integer
 * - %p: pointer
 *
 * @ret: The number of written bytes
 */
static int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap) {
	size_t count = 0;
	char *s;
	int i;
	int padding = 0;

	while (*fmt && count < size) {
		if (*fmt == '%') {
			fmt++;
			if (*fmt >= '0' && *fmt <= '9')
				padding = *fmt++ - '0';
			switch (*fmt) {
				case '%':
					buf[count++] = '%';
					break;
				case 'c':
					buf[count++] = (char)va_arg(ap, int);
					break;
				case 's':
					s = va_arg(ap, char *);
					while (*s && count < size)
						buf[count++] = *s++;
					break;
				case 'x':
					i = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, i, 16, 0, padding);
					break;
				case 'p':
					if (size - count > 2) {
						i = va_arg(ap, int);
						buf[count++] = '0';
						buf[count++] = 'x';
						count += snprintnum(buf + count, size - count, i, 16, 0, padding);
					}
					break;
				case 'b':
					i = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, i, 2, 0, padding);
					break;
				case 'd':
					i = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, i, 10, 1, padding);
					break;
				case 'u':
					i = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, i, 10, 0, padding);
					break;
				default:
					return count;
			}
			fmt++;
		} else {
			buf[count++] = *fmt++;
		}
	}
	return count;
}

/*
 * Wrapper to vsnprintf that calls it with a va_list
 * 
 * @ret: Number of written bytes
 */
int snprintf(char *buf, size_t size, const char *fmt, ...) {
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(buf, size, fmt, ap);
	va_end(ap);

	return ret;
}

/*
 * Wrapper to vsnprintf that writes at most BUFLEN bytes into the VGA buffer
 * 
 * @ret: Number of written bytes
 */
int kprintf(const char *fmt, ...) {
	char buf[BUFSIZE] = {0};
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(buf, BUFSIZE - 1, fmt, ap);
	va_end(ap);
	
	sb_putstr(sb + sb_index, buf);
	return ret;
}
