/* include/kernel/screenbuf.h
 *
 * Insert file description here
 *
 * created: 2022/10/19 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - xlmod <glafond-@student.42.fr>
 */

#ifndef SCREENBUF_H
#define SCREENBUF_H

#include <kernel/vga.h>

#define SB_HEIGHT		VGA_HEIGHT * 8

#define SB_WHITESPACE	(uint16_t)0x0f00

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
void sb_set_fg(struct screenbuf *sb, enum vga_color fg);
void sb_set_bg(struct screenbuf *sb, enum vga_color bg);
uint8_t sb_get_color(struct screenbuf *sb);

#endif
