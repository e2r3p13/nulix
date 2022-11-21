// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* keyboard.c
 *
 * Keyboard driver
 *
 * created: 2022/10/15 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/21 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/keyboard.h>
#include <kernel/port.h>
#include <kernel/ps2.h>
#include <kernel/vga.h>
#include <stdint.h>

/* Used to translate scancodes to keycodes. Defined in scancodes.c */
extern char kbd_ext_keycodes[];
extern char kbd_reg_keycodes[];

struct kbd_state kbd_st = {0};

/*
 * Set the LED state of the capslock key
 *
 * @on: 1 to turn on the LED, 0 to turn off
 */
static void set_capslock_led(int on) {
	//TODO
}

/*
 * Set the LED state of the numlock key
 *
 * @on: 1 to turn on the LED, 0 to turn off
 */
static void set_numlock_led(int on) {
	//TODO
}

/*
 * Initializes the keyboard, including the keymap layout to
 * be used, turning off the LEDS...
 * Should be called before any use of the keyboard
 *
 */
void KBD_initialize() {
	set_capslock_led(0);
	set_numlock_led(0);
	KBD_setkeymap(US104_getkeymapentry);
}

/*
 * Poll function used to wait for keyboard input
 * Meant to be replaced when interrupts will be implemented
 *
 * Returns 1 if some keyboard data is available, 0 otherwise
 */
int KBD_poll() {
	uint8_t status = port_read_u8(PS2_STATUS_REGISTER_PORT);

	return status & 1;
}

/*
 * Updates the modifiers of the keyboard state according to
 * the given event if it is relevant
 *
 * @evt: the event that may be a modifier event
 */
static void set_modifiers(struct kbd_event *evt) {
	switch (evt->key) {
		case KEY_LEFT_SHIFT:
			kbd_st.modifiers.lshift = (evt->type == KEY_PRESSED);
			break;
		case KEY_RIGHT_SHIFT:
			kbd_st.modifiers.rshift = (evt->type == KEY_PRESSED);
			break;
		case KEY_CAPS_LOCK:
			if (evt->type == KEY_RELEASED) {
				kbd_st.modifiers.capslock = !kbd_st.modifiers.capslock;
				set_numlock_led(kbd_st.modifiers.capslock);
			}
			break;
		case KEY_NUMLOCK:
			if (evt->type == KEY_RELEASED) {
				kbd_st.modifiers.numlock = !kbd_st.modifiers.numlock;
				set_numlock_led(kbd_st.modifiers.numlock);
			}
			break;
		case KEY_LEFT_CTRL:
			kbd_st.modifiers.lctrl = (evt->type == KEY_PRESSED);
			break;
		case KEY_RIGHT_CTRL:
			kbd_st.modifiers.rctrl = (evt->type == KEY_PRESSED);
			break;
		case KEY_LEFT_ALT:
			kbd_st.modifiers.lalt = (evt->type == KEY_PRESSED);
			break;
		case KEY_RIGHT_ALT:
			kbd_st.modifiers.ralt = (evt->type == KEY_PRESSED);
			break;
	}
}

/*
 * Gets a keyboard event, reading the PS/2 Data port
 *
 * @evt is a pointer to a kbd_event structure which is
 * to be filled
 *
 * Returns:
 *   0 if an event is available (a key has been pressed or released)
 *  -1 if an error occured
 *  -2 if an extended keycode is waited for
 *
 */
int KBD_getevent(struct kbd_event *evt) {
	uint8_t sc = port_read_u8(PS2_DATA_PORT);

	if (sc == 0xE0) {
		if (kbd_st.ext)
			return -1;
		kbd_st.ext = 1;
		return -2;
	} 

	if (sc > 0x80) {
		evt->type = KEY_RELEASED;
		sc -= 0x80;
	} else {
		evt->type = KEY_PRESSED;
	}

	if (kbd_st.ext)
		evt->key = kbd_ext_keycodes[sc];
	else
		evt->key = kbd_reg_keycodes[sc];

	set_modifiers(evt);
	kbd_st.ext = 0;

	return evt->key == UNKNOWN ? -1 : 0;
}

/*
 * Gets the character corresponding to the given event,
 * according to the keyboard state (modifiers/layout)
 *
 * @evt: The event to inspect
 *
 * Returns the ascii value of the character,
 * or 0 if there is no printable character for that event
 */
int KBD_getchar(struct kbd_event *evt) {
	struct kbd_keymap_entry ent;
	struct kbd_modifiers *mod = &kbd_st.modifiers;
	int c;

	if (!kbd_st.getkeymapentry)
		return 0;

	ent = kbd_st.getkeymapentry(evt->key);
	if (!ent.reg)
		return 0;
	if (HAS_NO_MODIFIERS(ent))
		return ent.reg;
	if (mod->ralt)
		return ent.altgr;
	if (mod->capslock && HAS_CAPSLOCK_EFFECT(ent))
		return mod->lshift | mod->rshift ? ent.reg : ent.shift;
	return mod->lshift | mod->rshift ? ent.shift : ent.reg;
}

/*
 * Change the keyboard layout by updating it's getkeymapentry field
 * Available keymaps can be found in the keymaps folder
 *
 */
void KBD_setkeymap(struct kbd_keymap_entry (*kmfn)(enum kbd_keycode)) {
	kbd_st.getkeymapentry = kmfn;
}

/*
 * Cleanup the PS/2 register port as recommended here:
 * https://wiki.osdev.org/%228042%22_PS/2_Controller
 *
 */
void KBD_flush() {
	uint8_t status = port_read_u8(PS2_STATUS_REGISTER_PORT);
	port_write_u8(PS2_STATUS_COMMAND_PORT, status & (0xff << 2));
}
