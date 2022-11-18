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
#include <stdint.h>
#include <kernel/vga.h>
#include <kernel/keyboard.h>
#include <kernel/print.h>

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

/* Hexdump to print from address to limit
 *
 * @base is the address to start the print from
 * @limit is the size we want to print from @base
 *
 */
void hexdump(uint32_t *base, uint32_t limit)
{
	uint32_t i = 0;
	uint8_t line = 0;
	struct kbd_event evt;
	
	VGA_clear();
	KBD_flush(&evt);
	while (i < limit)
	{
		if (line == VGA_HEIGHT)
		{
			line = 0;
			while (!KBD_poll());
			KBD_getevent(&evt);
			if (evt.type != KEY_PRESSED) {
				KBD_flush(&evt);
				continue;
			}
		}
		VGA_setforegroundcolor(VGA_COLOR_WHITE);
		kprintf("0x%x:  ", base);
		uint32_t *next = base + 16;
		uint32_t *tmp = base;
		
		while (tmp < next)
		{
			if (*(uint8_t*)tmp > 32)
			{
				VGA_setforegroundcolor(VGA_COLOR_BLUE);
				kprintf("%x ", *(uint8_t*)tmp);
				VGA_setforegroundcolor(VGA_COLOR_WHITE);
			}
			else
				kprintf("00 ");
			tmp++;
		}
		next = base + 16;
		tmp = base;
		while (tmp < next)
		{
			if (*(uint8_t*)tmp > 32)
			{
				VGA_setforegroundcolor(VGA_COLOR_BLUE);
				kprintf("%c", *(uint8_t *)tmp);
				VGA_setforegroundcolor(VGA_DFL_COLOR);
			}
			else
				kprintf(".");
			tmp++;
		}
		VGA_setforegroundcolor(VGA_DFL_COLOR);
		kprintf("\n");
		i += 16;
		base += 16;
		line++;
	} 
}
