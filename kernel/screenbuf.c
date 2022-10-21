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

/* Return pointer incremented by a value on circular buffer
 *
 *  return a + b
 */
static inline uint16_t *sb_get_ptraddline(struct screenbuf *sb, uint16_t *a, int b) {
	uint16_t val = b * VGA_WIDTH;
	if (a + val < sb->end) {
		return a + val;
	} else {
		return sb->buf + val - (sb->end - a);
	}
}

/* Return pointer decremented by a value on circular buffer
 *
 *  return a - b
 */
static inline uint16_t *sb_get_ptrsubline(struct screenbuf *sb, uint16_t *a, int b) {
	uint16_t val = b * VGA_WIDTH;
	if (a - val >= sb->top) {
		return a - val;
	} else {
		return sb->end - (val - (a - sb->buf));
	}
}

/* Put cursor on next line and scroll buffer by one and reset the cursor offset
 *
 */
static inline void *sb_next_line(struct screenbuf *sb) {
	sb->cursor = sb_get_ptraddline(sb, sb->cursor, 1);
	if (sb->cursor == sb->top) {
		sb->top = sb_get_ptraddline(sb, sb->top, 1);
		if (sb->current == sb->cursor)
			sb->current = sb->top;
	}
	memset(sb->cursor, 0, VGA_WIDTH * 2);
	sb->cursor_offset = 0;
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
	sb->end = sb->buf + (VGA_WIDTH * SCREENBUF_HEIGHT);
	sb->loaded = 0;
}

/* Load a part of the screen buffer to the vga buffer
 *
 */
void sb_load(struct screenbuf *sb) {
	sb->loaded = 1;


	for (uint16_t i = 0; i < VGA_HEIGHT; i++) {
		uint16_t *ptr = sb_get_ptraddline(sb, sb->current, i);
		uint16_t offset = i * VGA_WIDTH;
		if (ptr != sb->cursor)
			memcpy(VGA_PTR + offset, ptr, VGA_WIDTH * 2);
		else {
			memcpy(VGA_PTR + offset, ptr, sb->cursor_offset * 2);
			memset(VGA_PTR + offset + sb->cursor_offset, 0, (VGA_WIDTH - sb-> cursor_offset) * 2); 
			memset(VGA_PTR + offset + VGA_WIDTH, 0, ((VGA_HEIGHT - 1) * VGA_WIDTH * 2) - (i * VGA_WIDTH * 2)); 
			break;
		}
	}


//	if (sb->current + VGA_BYTES <= sb->end) {
//		if (sb->current <= sb->cursor) {
//			ptrdiff_t tocursor = sb->cursor - sb->current;
//			memcpy(VGA_PTR, sb->current, tocursor * 2);
//			memset(VGA_PTR + tocursor, 0, (VGA_BYTES - tocursor) * 2);
//		} else {
//			memcpy(VGA_PTR, sb->current, VGA_BYTES * 2);
//		}
//	} else {
//		ptrdiff_t toend = sb->end - sb->current;
//		ptrdiff_t tocursor = sb->cursor - sb->buf;
//		memcpy(VGA_PTR, sb->current, toend * 2);
//		if (tocursor > VGA_BYTES -toend) {
//			memcpy(VGA_PTR + toend, sb->buf, (VGA_BYTES -toend) * 2);
//		} else {
//			memcpy(VGA_PTR + toend, sb->buf, tocursor * 2);
//			memset(VGA_PTR + toend + tocursor, 0, (VGA_BYTES - (toend + tocursor)) * 2);
//		}
//	}
}

/* Scroll up the screen buffer by nbline and update vga buffer is this
 * buffer is loaded
 *
 */
void sb_scrollup(struct screenbuf *sb, int nbline) {
	int nb = nbline > (sb_get_ptrdiff(sb, sb->top, sb->current) / VGA_WIDTH) ? -1 : nbline;
	if (nb < 0) {
		sb->current = sb->top;
	} else {
		memset(sb->current , 0x20, 2);
		sb->current = sb_get_ptrsubline(sb, sb->current, nb);
	}
	if (sb->loaded)
		sb_load(sb);
}

/* Scroll down the screen buffer by nbline and update vga buffer is this
 * buffer is loaded
 *
 */
void sb_scrolldown(struct screenbuf *sb, int nbline) {
	ptrdiff_t diff = sb_get_ptrdiff(sb, sb->current, sb->cursor) / VGA_WIDTH;
	int nb = nbline > diff - VGA_HEIGHT ? -1 : nbline;
	if (nb < 0) {
		if (diff >= VGA_HEIGHT)
			sb->current = sb_get_ptrsubline(sb, sb->cursor, VGA_HEIGHT - 1);
	} else {
		sb->current = sb_get_ptraddline(sb, sb->current, nb);
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
	sb->cursor = sb->top;
	sb->cursor_offset = 0;
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
			sb_next_line(sb);
			break;
		default:
			*(sb->cursor + sb->cursor_offset) = ((uint16_t)(c) | (uint16_t)(color << 8));
			if (sb->loaded) {
			}
			sb->cursor_offset++;
			if (sb->cursor_offset == VGA_WIDTH)
				sb_next_line(sb);
			else {
				if (sb->loaded) {
					diff = sb_get_ptrdiff(sb, sb->current, sb->cursor);
					VGA_putentrydirect(c, color, diff + sb->cursor_offset);
				}
			}
			break;
	}
}

/* Print a string to the buffer and in the vga buffer if is loaded
 *
 */
void sb_write_str(struct screenbuf *sb, char *str, uint8_t color) {
	while (*str) {
		sb_write_char(sb, *str++, color);
	}
}
