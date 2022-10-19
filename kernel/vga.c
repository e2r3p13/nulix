// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* vga.c
 *
 * Implements some basic function to interact with the VGA buffer
 *
 * created: 2022/10/13 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/vga.h>
#include <kernel/string.h>
#include <stdint.h>
#include <kernel/port.h>

struct vga vga = {
	.color = VGA_DFL_COLOR,
	.buffer = (uint16_t *)0xB8000,
};

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
	return (uint16_t)c | (uint16_t)color << 8;
}

static void update_cursor() {
	uint16_t pos;

	if (vga.x >= 0 && vga.x < VGA_WIDTH && vga.y >= 0 && vga.y < VGA_HEIGHT) {
		pos = vga.y * VGA_WIDTH + vga.x;
		port_write_u8(0x3D4, 0x0F);
		port_write_u8(0x3D5, (uint8_t) (pos & 0xFF));
		port_write_u8(0x3D4, 0x0E);
		port_write_u8(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
	}
}

/*
 * Wrapper to VGA_clear, kept for furure, if we want to handle special
 * things at start
 *
 */
void VGA_initialize(void) {
	VGA_clear();
}

/*
 * Moves the cursor to the specified position
 *
 * @x: The column index of the VGA buffer to move the cursor to
 * @y: The row index of the VGA buffer to move the cursor to
 */
void VGA_move_cursor_to(int x, int y) {
	if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
		vga.x = x;
		vga.y = y;
		update_cursor();
	}
}

/*
 * Moves the cursor by x/y offsets
 *
 * @x: Moves the cursor up to @x cells to the right
 * @y: Moves the cursor up to @yx cells to the bottom
 */
void VGA_move_cursor_by(int x, int y) {
	//TODO
}

/*
 * Sets the VGA color
 *
 * @fg: Foregroud color to be set
 * @bg: Background color to be set
 */
void VGA_setcolor(enum vga_color fg, enum vga_color bg) {
	vga.color = fg | bg << 4;
}

/*
 * Get an integer representing the specified color
 *
 * @fg: Foregroud part of the color
 * @bg: Background part of the color
 */
uint8_t VGA_getcolor(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

/*
 * Changes the background part of the VGA color
 * 
 * @bg: Background color to be set
 */
void VGA_setbackgroundcolor(enum vga_color bg) {
	vga.color = (vga.color & 0x0f) | bg << 4;
}

/*
 * Changes the foreground part of the VGA color
 * 
 * @fg: Foregroud color to be set
 */
void VGA_setforegroundcolor(enum vga_color fg) {
	vga.color = (vga.color & 0xf0) | fg;
}

/*
 * Moves all lines @n lines higher (erasing the @n first lines),
 * and sets the last @n lines to spaces
 * Also updates the cursor position
 *
 */
void VGA_scrollby(size_t n) {
	for (size_t y = n; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			vga.buffer[(y - n) * VGA_WIDTH + x] = vga.buffer[y * VGA_WIDTH + x];
		}
	}
	for (size_t y = VGA_HEIGHT - n; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			vga.buffer[y * VGA_WIDTH + x] = '\0';
		}
	}
	vga.y = n > vga.y ? 0 : vga.y - n;
}

/*
 * Clears the VGA buffer, replacing all characters by spaces
 *
 */
void VGA_clear(void) {
	for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
		vga.buffer[i] = vga_entry(' ', VGA_DFL_COLOR);
	vga.x = 0;
	vga.y = 0;
	update_cursor();
}

/*
 * Writes a character at a specified position with a specified color
 *
 * @c: Character to be written
 * @color: Color of the character to be written
 * @x: Column of thw character to be written at
 * @y: Row of thw character to be written at
 *
 * NOTE: Does not updates the cursor position
 */
void VGA_putentryat(char c, uint8_t color, size_t x, size_t y) {
	size_t i;
	
	if (x < VGA_WIDTH && y < VGA_HEIGHT) {
		i = y * VGA_WIDTH + x;
		vga.buffer[i] = vga_entry(c, color);
	}
}

/*
 * Writes a character at buf + s with a specified color
 *
 * @c: Character to be written
 * @color: Color of the character to be written
 * @s: Offset of the character to be written at
 */
void VGA_putentrydirect(char c, uint8_t color, size_t s) {
	size_t i;
	
	if (s < VGA_WIDTH * VGA_HEIGHT) {
		vga.buffer[s] = vga_entry(c, color);
	}
}


/*
 * Writes a character to the cursor position and updates it.
 *
 * @c: Character to be written
 */
void VGA_putchar(char c) {
	switch (c) {
		case '\n':
			vga.x = 0;
			vga.y++;
			if (vga.y == VGA_HEIGHT)
				VGA_scrollby(1);
			update_cursor();
			break;
		case '\b':
			vga.x--;
			if (vga.x < 0) {
				vga.x = VGA_WIDTH - 1;
				vga.y--;
				if (vga.y < 0) {
					vga.x = 0;
					vga.y = 0;
				}
			}
			update_cursor();
			VGA_putentryat(' ', vga.color, vga.x, vga.y);
			break;
		default:
			VGA_putentryat(c, vga.color, vga.x, vga.y);
			vga.x++;
			if (vga.x == VGA_WIDTH) {
				vga.x = 0;
				vga.y++;
			}
			if (vga.y == VGA_HEIGHT)
				VGA_scrollby(1);
			update_cursor();
			break;
	}
}

/*
 * Writes data to the buffer and updates the cursor position
 *
 * @data: Data to be written
 * @size: Size of the data to be written
 */
void VGA_write(const char *data, size_t size) {
	while (size--)
		VGA_putchar(*data++);
}

void VGA_setbuf(uint16_t *data) {
	memcpy(vga.buffer, data, sizeof(uint16_t) * VGA_WIDTH * VGA_HEIGHT);
}

uint16_t *VGA_getbuf() {
	return vga.buffer;
}

/*
 * Writes a null terminated string on the VGA buffer and updates the
 * cursor position
 *
 * @str: String to be written
 */
void VGA_writestring(const char *str) {
	while (*str)
		VGA_putchar(*str++);
}
