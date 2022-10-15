#include <stdarg.h>
#include <kernel/string.h>
#include <kernel/vga.h>

#define BUFSIZE 4096

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
static int snprintnum(char *buf, size_t size, int n, int base, int issigned) {
	const char digits[] = "0123456789abcdef";
	unsigned int un;
	size_t count = 0;
	int nl;

	if (base > 16 || size == 0)
		return 0;
	if (n == 0) {
		buf[count++] = '0';
		return count;
	}

	if (issigned && n < 0) {
		buf++[count++] = '-';
		un = -n;
	} else {
		un = n;
	}

	nl = basepow(un, base);
	while (un != 0) {
		nl--;
		if (count + nl < size) {
			buf[nl] = digits[un % base];
			count++;
		}
		un /= base;
	}
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

	while (*fmt && count < size) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case '\0':
					return count;
				case '%':
					buf[count++] = '%';
					break;
				case 'c':
					buf[count++] = (char)va_arg(ap, int);
					break;
				case 's':
					const char *s = va_arg(ap, char *);
					while (*s && count < size)
						buf[count++] = *s++;
					break;
				case 'x':
					const int x = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, x, 16, 0);
					break;
				case 'p':
					if (size - count > 2) {
						const int p = va_arg(ap, int);
						buf[count++] = '0';
						buf[count++] = 'x';
						count += snprintnum(buf + count, size - count, p, 16, 0);
					}
					break;
				case 'b':
					const int b = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, b, 2, 0);
					break;
				case 'd':
					const int d = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, d, 10, 1);
					break;
				case 'u':
					const int u = va_arg(ap, int);
					count += snprintnum(buf + count, size - count, u, 10, 0);
					break;
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
	
	VGA_writestring(buf);
	return ret;
}
