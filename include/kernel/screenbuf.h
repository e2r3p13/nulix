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

#define SCREENBUF_WIDTH		VGA_WIDTH 
#define SCREENBUF_HEIGHT	VGA_HEIGHT * 8

struct screenbuf {
	uint16_t	*top;
	uint16_t	*end;
	uint16_t	*current;
	uint16_t	*cursor;
	uint16_t	cursor_offset; 
	int			loaded;
	uint16_t	buf[SCREENBUF_WIDTH * SCREENBUF_HEIGHT];
};

void sb_init(struct screenbuf *sb);
void sb_load(struct screenbuf *sb);

void sb_scrollup(struct screenbuf *sb, int nbline);
void sb_scrolldown(struct screenbuf *sb, int nbline);
void sb_clear(struct screenbuf *sb);

void sb_write_char(struct screenbuf *sb, char c, uint8_t color);
void sb_write_str(struct screenbuf *sb, char *str, uint8_t color);

#endif
