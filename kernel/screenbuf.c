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
	if (a < b) {
		return b - a;
	} else {
		return (sb->end - a) + (b - sb->buf);
	}
}

/* Return pointer incremented by a value on circular buffer
 *
 *  return a + b
 */
static inline uint16_t *sb_get_ptradd(struct screenbuf *sb, uint16_t *a, int b) {
	if (a + b < sb->end) {
		return a + b;
	} else {
		return sb->buf + b - (sb->end - a);
	}
}

/* Return pointer decremented by a value on circular buffer
 *
 *  return a - b
 */
static inline uint16_t *sb_get_ptrsub(struct screenbuf *sb, uint16_t *a, int b) {
	if (a - b > sb->top) {
		return a - b;
	} else {
		return sb->end - (b - (a - sb->buf));
	}
}

/* Initialize the screen buffer
 *
 */
void sb_init(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->current = sb->buf;
	sb->cursor = sb->buf;
	sb->end = sb->buf + (SCREENBUF_WIDTH * SCREENBUF_HEIGHT);
	sb->loaded = 0;
}

/* Load a part of the screen buffer to the vga buffer
 *
 */
void sb_load(struct screenbuf *sb) {
	sb->loaded = 1;
	if (sb->current + VGA_BYTES <= sb->end) {
		if (sb->current <= sb->cursor) {
			ptrdiff_t tocursor = sb->cursor - sb->current;
			memcpy(VGA_PTR, sb->current, tocursor * 2);
			memset(VGA_PTR + tocursor, 0, (VGA_BYTES - tocursor) * 2);
		} else {
			memcpy(VGA_PTR, sb->current, VGA_BYTES * 2);
		}
	} else {
		ptrdiff_t toend = sb->end - sb->current;
		ptrdiff_t tocursor = sb->cursor - sb->buf;
		memcpy(VGA_PTR, sb->current, toend * 2);
		if (tocursor > VGA_BYTES -toend) {
			memcpy(VGA_PTR + toend, sb->buf, (VGA_BYTES -toend) * 2);
		} else {
			memcpy(VGA_PTR + toend, sb->buf, tocursor * 2);
			memset(VGA_PTR + toend + tocursor, 0, (VGA_BYTES - (toend + tocursor)) * 2);
		}
	}
}

/* Scroll up the screen buffer by nbline and update vga buffer is this
 * buffer is loaded
 *
 */
void sb_scrollup(struct screenbuf *sb, int nbline) {
	int nb = nbline > sb_get_ptrdiff(sb, sb->top, sb->current) / SCREENBUF_WIDTH ? -1 : nbline;
	if (nb < 0) {
		sb->current = sb->top;
	} else {
		sb->current = sb_get_ptrsub(sb, sb->current, nb * SCREENBUF_WIDTH);
	}
	if (sb->loaded)
		sb_load(sb);
}

/* Scroll down the screen buffer by nbline and update vga buffer is this
 * buffer is loaded
 *
 */
void sb_scrolldown(struct screenbuf *sb, int nbline) {
	ptrdiff_t diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
	int nb = nbline > diff / SCREENBUF_WIDTH ? -1 : nbline;
	if (nb < 0) {
		if (diff / SCREENBUF_WIDTH > VGA_HEIGHT)
			sb->current = sb_get_ptradd(sb, sb->cursor, VGA_HEIGHT - (VGA_WIDTH - (diff % VGA_WIDTH)));
	} else {
		sb->current = sb_get_ptradd(sb, sb->current, nb * SCREENBUF_WIDTH);
	}
	if (sb->loaded)
		sb_load(sb);
}

/* Clear the screen buffer and update vga buffer if this buffer is loaded
 *
 */
void sb_clear(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->current = sb->top;
	sb->cursor = sb->top + 1;
	if (sb->loaded)
		sb_load(sb);
}

/* Print a char to the buffer and in the vga buffer if is loaded
 *
 */
void sb_write_char(struct screenbuf *sb, char c, uint8_t color) {
	ptrdiff_t	diff;
	uint16_t	*tmp;
	switch (c) {
		case '\n':
			diff = sb_get_ptrdiff(sb, sb->buf, sb->cursor);
			tmp = sb_get_ptradd(sb, sb->cursor, SCREENBUF_WIDTH - (diff % SCREENBUF_WIDTH));
			diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
			memset(VGA_PTR + diff, 0, sb_get_ptrdiff(sb, sb->cursor, tmp) * 2);
			sb->cursor = tmp;
			break;
		default:
			diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
			*sb->cursor = ((uint16_t)(c) | (uint16_t)(color << 8));
			if (sb->loaded) {
				VGA_putentrydirect(c, color, diff);
			}
			sb->cursor = sb_get_ptradd(sb, sb->cursor, 1);
			break;
	}
	diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
	if (diff >= VGA_BYTES)
		sb_scrolldown(sb, 1);
}

/* Print a string to the buffer and in the vga buffer if is loaded
 *
 */
void sb_write_str(struct screenbuf *sb, char *str, uint8_t color) {
	while (*str) {
		sb_write_char(sb, *str++, color);
	}
}
