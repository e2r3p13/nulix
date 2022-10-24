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

struct screenbuf {
	uint16_t		*top;
	uint16_t		*end;
	uint16_t		*current;
	uint16_t		*cursor;
	uint16_t		cursor_offset;
	uint8_t			color;
	int				loaded;
	uint16_t		buf[VGA_WIDTH * SB_HEIGHT];
};

void sb_init(struct screenbuf *sb);
void sb_load(struct screenbuf *sb);
void sb_unload(struct screenbuf *sb);

void sb_scrollup(struct screenbuf *sb, int nbline);
void sb_scrolldown(struct screenbuf *sb, int nbline);
void sb_clear(struct screenbuf *sb);

void sb_write_char(struct screenbuf *sb, char c);
void sb_write_str(struct screenbuf *sb, char *str);

void sb_set_color(struct screenbuf *sb, uint8_t color);
void sb_set_fg(struct screenbuf *sb, enum vga_color fg);
void sb_set_bg(struct screenbuf *sb, enum vga_color bg);

#endif
