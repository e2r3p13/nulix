// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* vga.h
 *
 * VGA driver header file
 *
 * created: 2022/10/15 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef VGA_H
#define VGA_H

#include <stddef.h>
#include <stdint.h>

#define VGA_DFL_COLOR 7 // light grey on black

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_PTR		(uint16_t *)0xB8000
#define VGA_BYTES	VGA_WIDTH * VGA_HEIGHT

struct vga {
	int x;
	int y;
	uint8_t color;
	uint16_t *buffer;
};

void VGA_initialize(void);
void VGA_clear(void);
void VGA_setcolor(enum vga_color fg, enum vga_color bg);
uint8_t VGA_getcolor(enum vga_color fg, enum vga_color bg);
void VGA_setbackgroundcolor(enum vga_color bg);
void VGA_setforegroundcolor(enum vga_color fg);
void VGA_scrollby(size_t n);
void VGA_putentryat(char c, uint8_t color, size_t x, size_t y);
void VGA_putentrydirect(char c, uint8_t color, size_t s);
void VGA_putchar(char c);
void VGA_write(const char *data, size_t size);
void VGA_writestring(const char *str);
void VGA_move_cursor_by(int x, int y);
void VGA_move_cursor_to(int x, int y);
void VGA_setbuf(uint16_t *data);
uint16_t *VGA_getbuf();

#endif
