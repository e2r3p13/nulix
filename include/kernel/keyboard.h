// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* keyboard.h
 *
 * Header file for the keyboard driver
 *
 * created: 2022/10/15 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* Offset between corresponding key pressed/key released scancodes */
#define KEY_RELEASE_OFFSET 0x80

enum kbd_eventtype {
	KEY_PRESSED,
	KEY_RELEASED,
};

enum kbd_keycode {
	UNKNOWN = 0,
	KEY_ESCAPE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_DASH,
	KEY_EQUAL,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_OPENING_SQUARE_BRACKET,
	KEY_CLOSING_SQUARE_BRACKET,
	KEY_ENTER,
	KEY_LEFT_CTRL,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_COLON,
	KEY_SINGLE_QUOTE,
	KEY_BACKTICK,
	KEY_LEFT_SHIFT,
	KEY_BACKSLASH,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_COMMA,
	KEY_DOT,
	KEY_SLASH,
	KEY_RIGHT_SHIFT,
	KEY_KEYPAD_WILDCARD,
	KEY_LEFT_ALT,
	KEY_SPACE,
	KEY_CAPS_LOCK,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_NUMLOCK,
	KEY_SCROLLLOCK,
	KEY_KEYPAD_7,
	KEY_KEYPAD_8,
	KEY_KEYPAD_9,
	KEY_KEYPAD_DASH,
	KEY_KEYPAD_4,
	KEY_KEYPAD_5,
	KEY_KEYPAD_6,
	KEY_KEYPAD_PLUS,
	KEY_KEYPAD_1,
	KEY_KEYPAD_2,
	KEY_KEYPAD_3,
	KEY_KEYPAD_0,
	KEY_KEYPAD_DOT,
	KEY_F11,
	KEY_F12,
	KEY_MULTIMEDIA_PREVIOUS_TRACK,
	KEY_MULTIMEDIA_NEXT_TRACK,
	KEY_KEYPAD_ENTER,
	KEY_RIGHT_CTRL,
	KEY_MULTIMEDIA_MUTE,
	KEY_MULTIMEDIA_CALCULATOR,
	KEY_MULTIMEDIA_PLAY,
	KEY_MULTIMEDIA_STOP,
	KEY_MULTIMEDIA_VOLUME_DOWN,
	KEY_MULTIMEDIA_VOLUME_UP,
	KEY_MULTIMEDIA_HOME_WWW,
	KEY_KEYPAD_SLASH,
	KEY_RIGHT_ALT,
	KEY_HOME,
	KEY_CURSOR_UP,
	KEY_PAGE_UP,
	KEY_CURSOR_LEFT,
	KEY_CURSOR_RIGHT,
	KEY_END,
	KEY_CURSOR_DOWN,
	KEY_PAGE_DOWN,
	KEY_INSERT,
	KEY_DELETE,
	KEY_LEFT_GUI,
	KEY_RIGHT_GUI,
	KEY_APPS,
	KEY_ACPI_POWER,
	KEY_ACPI_SLEEP,
	KEY_ACPI_WAKE,
	KEY_MULTIMEDIA_WWW_SEARCH,
	KEY_MULTIMEDIA_WWW_FAVORITES,
	KEY_MULTIMEDIA_WWW_REFRESH,
	KEY_MULTIMEDIA_WWW_STOP,
	KEY_MULTIMEDIA_WWW_FORWARD,
	KEY_MULTIMEDIA_WWW_BACK,
	KEY_MULTIMEDIA_MY_COMPUTER,
	KEY_MULTIMEDIA_EMAIL,
	KEY_MULTIMEDIA_MEDIA_SELECT,
};

struct kbd_event {
	enum kbd_eventtype type;
	enum kbd_keycode key;
};

struct kbd_modifiers {
	int lshift : 1;
	int rshift : 1;
	int capslock : 1;
	int lctrl : 1;
	int rctrl : 1;
	int lalt : 1;
	int ralt : 1;
	int numlock : 1;
};

/*
 * kbd_state struct keeps information on the
 * keyboard context
 *
 * kbd_st is a global of that type
 *
 * @getkeymapentry points to the currently set keymap function to
 * retrieve the appropriate keymap entry
 *
 * @modifiers keeps track of the enabled modifier keys (capslock, shift, alt...)
 *
 * When @ext is enabled, the next byte to be read on the PS2 data port will be
 * proceeded as an extended scancode
 */
struct kbd_state {
	struct kbd_keymap_entry (*getkeymapentry)(enum kbd_keycode);
	struct kbd_modifiers modifiers;
	int ext;
};

/* keymap entry flags */
#define NO_MOD   0b100 /* No modifier has effect (regular form is used) */
#define CAPSLOCK 0b010 /* Capslock has effect (is xored with shift) */
#define NUMLOCK  0b001 /* Numlock has effect (but not used for now) */

/* Macros to check for keymap entry flags */
#define HAS_CAPSLOCK_EFFECT(x) (x.flags & 0b010)
#define HAS_NUMLOCK_EFFECT(x) (x.flags & 0b001)
#define HAS_NO_MODIFIERS(x) (x.flags & 0b100)

/*
 * kbd_keymap_entry contains information on which keycode
 * corresponds to which character for a specific keymap layout
 * with specific modifiers
 *
 * @flags: see keymap entry flags above
 * @reg: regular form of the keycode
 * @shift: keycode character when shift is pressed
 * @altgr: keycode character when altgr is pressed
 */
struct kbd_keymap_entry {
	uint8_t flags;
	uint8_t reg;
	uint8_t shift;
	uint8_t altgr;
};

void KBD_initialize();
int KBD_poll();
int KBD_getevent(struct kbd_event *evt);
int KBD_getchar(struct kbd_event *evt);
void KBD_flush();
void KBD_setkeymap(struct kbd_keymap_entry (*kmfn)(enum kbd_keycode));


/*************
 * Keymaps
 */

struct kbd_keymap_entry US104_getkeymapentry(enum kbd_keycode kc);	/* US */
struct kbd_keymap_entry FR_getkeymapentry(enum kbd_keycode kc);		/* FR */

#endif
