/* kernel/screenbuf.c
 *
 * Implementation of a circular buffer used to store
 * graphic information, and keep track of several metadata
 * (cursor, color, current view window...)
 *
 * created: 2022/10/19 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/19 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/port.h>
#include <kernel/screenbuf.h>
#include <kernel/string.h>

/*
 * TODO: Move this function to stdlib 
 */
static int abs(int x) {
	return x < 0 ? -x : x;
}

/*
 * Enable and show the cursor on the VGA buffer
 */
static void	cursor_enable() {
	port_write_u8(SB_CURSOR_PORT1, 0x0a);
	uint8_t cstart = port_read_u8(SB_CURSOR_PORT2) & 0xc0;
	port_write_u8(SB_CURSOR_PORT2, cstart | 0);

	port_write_u8(SB_CURSOR_PORT1, 0x0b);
	uint8_t cend = port_read_u8(SB_CURSOR_PORT2) & 0xe0;
	port_write_u8(SB_CURSOR_PORT2, cend | 15);
}

/*
 * Disable and hide the cursor from the VGA buffer
 */
static void cursor_disable() {
	port_write_u8(SB_CURSOR_PORT1, 0x0a);
	port_write_u8(SB_CURSOR_PORT2, 0x20);
}

/*
 * Enable or disable the cursor weither or not the
 * cursor position is into the VGA buffer area, and
 * set it's position to @pos
 */
static void cursor_update(uint32_t pos) {
	if (pos < VGA_WIDTH * SB_HEIGHT) {
		cursor_enable();
		port_write_u8(SB_CURSOR_PORT1, 0x0f);
		port_write_u8(SB_CURSOR_PORT2, (uint8_t)(pos & 0xff));
		port_write_u8(SB_CURSOR_PORT1, 0x0e);
		port_write_u8(SB_CURSOR_PORT2, (uint8_t)((pos >> 8) & 0xff));
	} else {
		cursor_disable();
	}
}

/*
 * Sets @n words of memory to @word from @ptr
 */
static void sb_memset(uint16_t *ptr, uint16_t word, size_t n) {
	while (n--)
		*ptr++ = word;
}

/*
 * Computes and returns the difference between @a and @b in bytes.
 * @a and @b must point inside @sb->buf.
 */
static ptrdiff_t sb_diff(struct screenbuf *sb, uint16_t *a, uint16_t *b) {
	if (a <= b)
		return b - a;
	else
		return (sb->endbuf - a) + (b - sb->buf);
}

/*
 * Computes and returns the difference between @a and @b in lines.
 * @a and @b must point inside @sb->buf.
 */
static inline ptrdiff_t sb_ldiff(struct screenbuf *sb, uint16_t *a, uint16_t *b) {
	return sb_diff(sb, a, b) / VGA_WIDTH;
}

/*
 * Returns a pointer @n lines ahead from @a in the circular buffer @sb
 * @n can be negative to get a pointer @n lines behind @a.
 */
static uint16_t *sb_addline(struct screenbuf *sb, uint16_t *a, int n) {
	uint16_t *newpos = a + n * VGA_WIDTH;
	size_t bufsize = sb->endbuf - sb->buf;

	while (newpos >= sb->endbuf)
		newpos -= bufsize;
	while (newpos < sb->buf)
		newpos += bufsize;
	return newpos;
}

/*
 * Inits the given screenbuf
 */
void sb_init(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->viewport = sb->buf;
	sb->cursor = sb->buf;
	sb->cursor_offset = 0;
	sb_memset(sb->buf, SB_WHITESPACE, VGA_WIDTH * SB_HEIGHT);
	sb->color = SB_DFL_COLOR;
	sb->endbuf = sb->buf + (VGA_WIDTH * SB_HEIGHT);
	sb->loaded = 0;
	sb_memset(sb->buf, SB_WHITESPACE, VGA_WIDTH * SB_HEIGHT);
}

/*
 * Synchronize the VGA buffer content with the given screenbuf if loaded
 */
static void sb_sync(struct screenbuf *sb) {
	uint32_t pos;

	if (sb->loaded) {
		for (uint16_t i = 0; i < VGA_HEIGHT; i++)
			memcpy(VGA_PTR + (i * VGA_WIDTH), sb_addline(sb, sb->viewport, i), VGA_WIDTH * 2);
		pos = sb_diff(sb, sb->viewport, sb->cursor) + sb->cursor_offset;
		cursor_update(pos);
	}
}

/*
 * Loads the given screenbuf to the VGA buffer
 */
void sb_load(struct screenbuf *sb) {
	sb->loaded = 1;
	sb_sync(sb);
}

/*
 * Unloads a screenbuffer
 */
void sb_unload(struct screenbuf *sb) {
	sb->loaded = 0;
}

/*
 * Sets the given screenbuf color. The first 4 bits corresponds
 * to the background, the 4 last ones to the foreground.
 */
void sb_set_color(struct screenbuf *sb, uint8_t color) {
	sb->color = color;
}

/*
 * Sets the given screenbuf's foreground color
 */
void sb_set_fg(struct screenbuf *sb, enum sb_color fg) {
	sb->color = (sb->color & 0xf0) | (fg & 0x0f);
}

/*
 * Sets the given screenbuf's background color
 */
void sb_set_bg(struct screenbuf *sb, enum sb_color bg) {
	sb->color = ((bg << 4) & 0xf0) | (sb->color & 0x0f);
}

/*
 * Returns the screenbuf's current color
 */
uint8_t sb_get_color(struct screenbuf *sb) {
	return sb->color;
}

/*
 * Makes the screenbuf scrolls @n line up or down and
 * updates the VGA buffer accordingly.
 * The buffer won't scroll upper than the first line of the
 * circular buffer, and won't scroll lower than the cursor
 * position.
 */
void sb_scroll(struct screenbuf *sb, int n) {
	int lines_room;

	lines_room = n < 0 ?
		sb_ldiff(sb, sb->top, sb->viewport) :
		sb_ldiff(sb, sb->viewport, sb->cursor) - (VGA_HEIGHT - 1);

	if (lines_room >= abs(n)) {
		sb->viewport = sb_addline(sb, sb->viewport, n);
		sb_sync(sb);
	} else {
		if (n < 0)
			sb_scroll_top(sb);
		else 
			sb_scroll_down(sb);
	}
}

/*
 * Scrolls to the top of the given circular buffer
 */
void sb_scroll_top(struct screenbuf *sb) {
	sb->viewport = sb->top;
	sb_sync(sb);
}

/*
 * Scrolls to the bottom of the given circular buffer
 */
void sb_scroll_down(struct screenbuf *sb) {
	if (sb_ldiff(sb, sb->viewport, sb->cursor) > VGA_HEIGHT) {
		sb->viewport = sb->cursor - (VGA_WIDTH * (VGA_HEIGHT - 1));
		sb_sync(sb);
	}
}

/*
 * Writes the word @word at the cursor position in the given screenbuf
 * If the screenbuf is loaded, also updates the VGA buffer accordingly
 * if the written word is in the visible window
 */
static void sb_write_word(struct screenbuf *sb, uint16_t word) {
	ptrdiff_t diff;

	*(sb->cursor + sb->cursor_offset) = word;
	if (sb->loaded) {
		diff = sb_diff(sb, sb->viewport, sb->cursor);
		if (diff / VGA_WIDTH < VGA_HEIGHT)
			*(VGA_PTR + diff + sb->cursor_offset) = word;
	}
}

/*
 * Adds a newline to the end of the buffer and moves
 * the cursor there.
 * As the screenbuffer is circular, it also moves the
 * buffer top one line down if needed.
 */
static void sb_crlf(struct screenbuf *sb) {
	sb->cursor_offset = 0;
	sb->cursor = sb_addline(sb, sb->cursor, 1);
	sb_memset(sb->cursor, SB_WHITESPACE, VGA_WIDTH);

	if (sb->cursor == sb->top) {
		if (sb->viewport == sb->top) {
			sb->viewport = sb_addline(sb, sb->top, 1);
			sb_sync(sb);
		}
		sb->top = sb_addline(sb, sb->top, 1);
	}
	if (sb_ldiff(sb, sb->viewport, sb->cursor) == VGA_HEIGHT)
		sb_scroll(sb, 1);
}

/*
 * Erases the last character if there is one
 */
static void sb_backspace(struct screenbuf *sb) {
	if (sb->cursor_offset == 0) {
		if (sb->cursor == sb->top)
			return;
		if (sb->viewport == sb->cursor)
			sb_scroll(sb, -1);
		sb->cursor_offset = 0;
		sb->cursor = sb_addline(sb, sb->cursor, -1);
		while (sb->cursor_offset < VGA_WIDTH && sb->cursor[sb->cursor_offset] != SB_WHITESPACE)
			sb->cursor_offset++;
	}
	sb->cursor_offset--;
	sb_write_word(sb, SB_WHITESPACE);
}

/*
 * Adds blank characters at the cursor position.
 * At least one blank character, and as much as needed
 * to be aligned to 4 characters are written.
 */
static void sb_tabulation(struct screenbuf *sb) {
	do {
		sb_write_word(sb, (sb->color << 8) | 0);
		sb->cursor_offset++;
	} while (sb->cursor_offset % TABSIZE && sb->cursor_offset < VGA_WIDTH);

	if (sb->cursor_offset == VGA_WIDTH)
		sb_crlf(sb);
}

/*
 * Writes a printable character onto the screenbuf, at the
 * cursor position, and on the VGA buffer if loaded
 * Updates the cursor positon
 */
static void sb_regular(struct screenbuf *sb, char c) {
	ptrdiff_t diff;

	sb->cursor[sb->cursor_offset++] = (uint16_t)(c) | (uint16_t)(sb->color << 8);
	if (sb->loaded) {
		diff = sb_diff(sb, sb->viewport, sb->cursor);
		memcpy(VGA_PTR + diff + sb->cursor_offset - 1, sb->cursor + sb->cursor_offset - 1, 2);
	}
	if (sb->cursor_offset == VGA_WIDTH)
		sb_crlf(sb);
}

/*
 * Writes the character @c to the given screenbuf
 * This function handles special characters
 */
void sb_putchar(struct screenbuf *sb, char c) {
	int newpos;

	switch (c) {
		case '\n':
			sb_crlf(sb);
			break;
		case '\t':
			sb_tabulation(sb);
			break;
		case '\b':
			sb_backspace(sb);
			break;
		default:
			sb_regular(sb, c);
			break;
	}
	newpos = sb_diff(sb, sb->viewport, sb->cursor) + sb->cursor_offset;
	cursor_update(newpos);
}

/*
 * Writes the string @s to the given screenbuf
 * This function handles special characters
 */
void sb_putstr(struct screenbuf *sb, char *s) {
	while (*s)
		sb_putchar(sb, *s++);
}

/*
 * Clears the screenbuf's content and update the VGA buffer
 * if @sb is loaded.
 */
void sb_clear(struct screenbuf *sb) {
	sb->top = sb->buf;
	sb->viewport = sb->buf;
	sb->cursor = sb->buf;
	sb->cursor_offset = 0;
	sb_memset(sb->buf, SB_WHITESPACE, VGA_WIDTH * SB_HEIGHT);
	sb_sync(sb);
}
