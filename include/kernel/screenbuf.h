/* include/kernel/screenbuf.h
 *
 * Insert file description here
 *
 * created: 2022/10/19 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - xlmod <glafond-@student.42.fr>
 */

#ifndef SCREENBUF_H
#define SCREENBUF_H

#include <stdint.h>

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_PTR		(uint16_t *)0xc00B8000

#define SB_HEIGHT		VGA_HEIGHT * 16

enum sb_color {
	SB_COLOR_BLACK = 0,
	SB_COLOR_BLUE = 1,
	SB_COLOR_GREEN = 2,
	SB_COLOR_CYAN = 3,
	SB_COLOR_RED = 4,
	SB_COLOR_MAGENTA = 5,
	SB_COLOR_BROWN = 6,
	SB_COLOR_LIGHT_GREY = 7,
	SB_COLOR_DARK_GREY = 8,
	SB_COLOR_LIGHT_BLUE = 9,
	SB_COLOR_LIGHT_GREEN = 10,
	SB_COLOR_LIGHT_CYAN = 11,
	SB_COLOR_LIGHT_RED = 12,
	SB_COLOR_LIGHT_MAGENTA = 13,
	SB_COLOR_LIGHT_BROWN = 14,
	SB_COLOR_WHITE = 15,
};

#define SB_DFL_FG_COLOR	SB_COLOR_WHITE
#define SB_DLF_BG_COLOR	SB_COLOR_BLACK
#define SB_DFL_COLOR	(uint8_t)((SB_DLF_BG_COLOR << 4) | SB_DFL_FG_COLOR)// light grey on black
#define SB_WHITESPACE	(uint16_t)(SB_DFL_COLOR << 8)

#define SB_CURSOR_PORT1	0x3D4
#define SB_CURSOR_PORT2	0x3D5

#define TABSIZE 4

struct screenbuf {
	uint16_t		*top;
	uint16_t		*viewport;
	uint16_t		*endbuf;
	uint16_t		*cursor;
	unsigned short	cursor_offset;
	uint8_t			color;
	int				loaded;
	uint16_t		buf[VGA_WIDTH * SB_HEIGHT];
};

void sb_init(struct screenbuf *sb);
void sb_load(struct screenbuf *sb);
void sb_unload(struct screenbuf *sb);

void sb_scroll(struct screenbuf *sb, int n);
void sb_scroll_top(struct screenbuf *sb);
void sb_scroll_down(struct screenbuf *sb);

void sb_putchar(struct screenbuf *sb, char c);
void sb_putstr(struct screenbuf *sb, char *s);
void sb_clear(struct screenbuf *sb);

void sb_set_color(struct screenbuf *sb, uint8_t color);
void sb_set_fg(struct screenbuf *sb, enum sb_color fg);
void sb_set_bg(struct screenbuf *sb, enum sb_color bg);
uint8_t sb_get_color(struct screenbuf *sb);

#endif
