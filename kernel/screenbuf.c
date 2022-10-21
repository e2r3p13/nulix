/* kernel/screenbuf.c
 *
 * Big buffer to keep writing and add true scroll
 *
 * created: 2022/10/19 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/screenbuf.h>
#include <kernel/string.h>
#include <kernel/vga.h>

/* Return difference between pointer on circular buffer
 *
 *  return b - a
 */
static inline ptrdiff_t sb_get_ptrdiff(struct screenbuf *sb, uint16_t *a, uint16_t *b) {
	if (a <= b) {
		return b - a;
	} else {
		return (sb->end - a) + (b - sb->buf);
	}
}

/* Return pointer incremented by b lines on circular buffer
 *
 *  return a + b * VGA_WIDTH
 */
static inline uint16_t *sb_get_ptraddline(struct screenbuf *sb, uint16_t *a, int b) {
	uint16_t val = b * VGA_WIDTH;
	if (a + val < sb->end) {
		return a + val;
	} else {
		return sb->buf + val - (sb->end - a);
	}
}

/* Return pointer decremented by b lines on circular buffer
 *
 *  return a - b * VGA_WIDTH
 */
static inline uint16_t *sb_get_ptrsubline(struct screenbuf *sb, uint16_t *a, int b) {
	uint16_t val = b * VGA_WIDTH;
	if (a - val >= sb->top) {
		return a - val;
	} else {
		return sb->end - (val - (a - sb->buf));
	}
}

/* If the buffer is loaded, copy a part of the screen buffer to the vga buffer
 *
 */
static inline void *sb_update(struct screenbuf *sb) {
	if (sb->loaded) {
		for (uint16_t i = 0; i < VGA_HEIGHT; i++)
			memcpy(VGA_PTR + (i * VGA_WIDTH), sb_get_ptraddline(sb, sb->current, i), VGA_WIDTH * 2);
	}
}

/* Put cursor on next line and scroll buffer by one and reset the cursor offset
 *
 */
static inline void *sb_next_line(struct screenbuf *sb) {
	sb->cursor = sb_get_ptraddline(sb, sb->cursor, 1);
	memset(sb->cursor, 0, VGA_WIDTH * 2);
	sb->cursor_offset = 0;
	if (sb->cursor == sb->top) {
		sb->top = sb_get_ptraddline(sb, sb->top, 1);
		if (sb->current == sb->cursor) {
			sb->current = sb->top;
			sb_update(sb);
		}
	}
	if (sb_get_ptrdiff(sb, sb->current, sb->cursor) / VGA_WIDTH == VGA_HEIGHT)
		sb_scrolldown(sb, 1);
}

/* Initialize the screen buffer
 *
 */
void sb_init(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->current = sb->buf;
	sb->cursor = sb->buf;
	sb->cursor_offset = 0;
	sb->color = VGA_DFL_COLOR;
	sb->end = sb->buf + (VGA_WIDTH * SCREENBUF_HEIGHT);
	sb->loaded = 0;
	memset(sb->buf, 0, VGA_WIDTH * SCREENBUF_HEIGHT * 2);
}

/* Load a part of the screen buffer to the vga buffer
 *
 */
void sb_load(struct screenbuf *sb) {
	sb->loaded = 1;
	sb_update(sb);
}

/* Unset the screen buffer. This no touch the vga buffer.
 *
 */
void sb_unload(struct screenbuf *sb) {
	sb->loaded = 0;
}

/* Scroll up the screen buffer by nbline and update vga buffer if this
 * buffer is loaded
 *
 */
void sb_scrollup(struct screenbuf *sb, int nbline) {
	int nb = nbline > (sb_get_ptrdiff(sb, sb->top, sb->current) / VGA_WIDTH) ? -1 : nbline;
	if (nb < 0)
		sb->current = sb->top;
	else
		sb->current = sb_get_ptrsubline(sb, sb->current, nb);
	sb_update(sb);
}

/* Scroll down the screen buffer by nbline and update vga buffer if this
 * buffer is loaded
 *
 */
void sb_scrolldown(struct screenbuf *sb, int nbline) {
	ptrdiff_t diff = sb_get_ptrdiff(sb, sb->current, sb->cursor) / VGA_WIDTH;
	int nb = nbline > diff - VGA_HEIGHT ? -1 : nbline;
	if (nb < 0) {
		if (diff >= VGA_HEIGHT)
			sb->current = sb_get_ptrsubline(sb, sb->cursor, VGA_HEIGHT - 1);
	} else
		sb->current = sb_get_ptraddline(sb, sb->current, nb);
	sb_update(sb);
}

/* Clear the screen buffer and update vga buffer if this buffer is loaded
 *
 */
void sb_clear(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->current = sb->top;
	sb->cursor = sb->top;
	sb->cursor_offset = 0;
	memset(sb->buf, 0, VGA_WIDTH * SCREENBUF_HEIGHT * 2);
	sb_update(sb);
}

/* Print a char to the buffer and in the vga buffer if is loaded
 *
 */
void sb_write_char(struct screenbuf *sb, char c) {
	ptrdiff_t	diff;
	uint16_t	*tmp;
	switch (c) {
		case '\n':
			sb_next_line(sb);
			break;
		default:
			*(sb->cursor + sb->cursor_offset) = ((uint16_t)(c) | (uint16_t)(sb->color << 8));
			sb->cursor_offset++;
			if (sb->cursor_offset == VGA_WIDTH)
				sb_next_line(sb);
			else {
				if (sb->loaded) {
					diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
					memcpy(VGA_PTR + diff + sb->cursor_offset - 1, sb->cursor + sb->cursor_offset - 1, 2);
				}
			}
			break;
	}
}

/* Print a string to the buffer and in the vga buffer if is loaded
 *
 */
void sb_write_str(struct screenbuf *sb, char *str) {
	while (*str) {
		sb_write_char(sb, *str++);
	}
}

/* Set the color of printing
 *
 */
void sb_set_color(struct screenbuf *sb, uint8_t color) {
	sb->color = color;
}

/* Set the foreground of printing
 *
 */
void sb_set_fg(struct screenbuf *sb, enum vga_color fg) {
	sb->color = (sb->color & 0xf0) | (fg & 0x0f);
}

/* Set the background of printing
 *
 */
void sb_set_bg(struct screenbuf *sb, enum vga_color bg) {
	sb->color = (bg & 0xf0) | (sb->color & 0x0f);
}
