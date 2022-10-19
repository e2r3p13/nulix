// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* keymaps/US104.c
 *
 * US104 keyboard layout
 *
 * created: 2022/10/17 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/10/19 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/keyboard.h>

/*
 * keymap entries are declared this way:
 *
 *      [Keycode]                           FLAGS              REG     SHIFT  ALT
 *
 */

struct kbd_keymap_entry US104_getkeymapentry(enum kbd_keycode kc) {
	static struct kbd_keymap_entry keymap[0xFF] = {
		[KEY_ESCAPE] 					= { NO_MOD           , 0x1B                },
		[KEY_1] 						= { 0                , '1'  ,  '!'  , '1'  },
		[KEY_2] 						= { 0                , '2'  ,  '@'  , '2'  },
		[KEY_3] 						= { 0                , '3'  ,  '#'  , '3'  },
		[KEY_4] 						= { 0                , '4'  ,  '$'  , '4'  },
		[KEY_5] 						= { 0                , '5'  ,  '%'  , '5'  },
		[KEY_6] 						= { 0                , '6'  ,  '^'  , '6'  },
		[KEY_7] 						= { 0                , '7'  ,  '&'  , '7'  },
		[KEY_8] 						= { 0                , '8'  ,  '*'  , '8'  },
		[KEY_9] 						= { 0                , '9'  ,  '('  , '9'  },
		[KEY_0] 						= { 0                , '0'  ,  ')'  , '0'  },
		[KEY_DASH] 						= { 0                , '-'  ,  '_'  ,  0   },
		[KEY_EQUAL] 					= { 0                , '='  ,  '+'  ,  0   },
		[KEY_BACKSPACE] 				= { NO_MOD           , '\b'                },
		[KEY_TAB]						= { NO_MOD           , '\t'                },
		[KEY_Q] 						= { CAPSLOCK         , 'q'  ,  'Q'  ,  0   },
		[KEY_W] 						= { CAPSLOCK         , 'w'  ,  'W'  ,  0   },
		[KEY_E] 						= { CAPSLOCK         , 'e'  ,  'E'  ,  0   },
		[KEY_R] 						= { CAPSLOCK         , 'r'  ,  'R'  ,  0   },
		[KEY_T] 						= { CAPSLOCK         , 't'  ,  'T'  ,  0   },
		[KEY_Y] 						= { CAPSLOCK         , 'y'  ,  'Y'  ,  0   },
		[KEY_U] 						= { CAPSLOCK         , 'u'  ,  'U'  ,  0   },
		[KEY_I] 						= { CAPSLOCK         , 'i'  ,  'I'  ,  0   },
		[KEY_O] 						= { CAPSLOCK         , 'o'  ,  'O'  ,  0   },
		[KEY_P] 						= { CAPSLOCK         , 'p'  ,  'P'  ,  0   },
		[KEY_OPENING_SQUARE_BRACKET] 	= { 0, '['           , '{'  ,          0   },
		[KEY_CLOSING_SQUARE_BRACKET] 	= { 0, ']'           , '}'  ,          0   },
		[KEY_ENTER] 					= { NO_MOD           , '\n'                },
		[KEY_A] 						= { CAPSLOCK         , 'a'  ,  'A'  ,  0   },
		[KEY_S] 						= { CAPSLOCK         , 's'  ,  'S'  ,  0   },
		[KEY_D] 						= { CAPSLOCK         , 'd'  ,  'D'  ,  0   },
		[KEY_F] 						= { CAPSLOCK         , 'f'  ,  'F'  ,  0   },
		[KEY_G] 						= { CAPSLOCK         , 'g'  ,  'G'  ,  0   },
		[KEY_H] 						= { CAPSLOCK         , 'h'  ,  'H'  ,  0   },
		[KEY_J] 						= { CAPSLOCK         , 'j'  ,  'J'  ,  0   },
		[KEY_K] 						= { CAPSLOCK         , 'k'  ,  'K'  ,  0   },
		[KEY_L] 						= { CAPSLOCK         , 'l'  ,  'L'  ,  0   },
		[KEY_COLON] 					= { 0                , ';'  ,  ':'  ,  0   },
		[KEY_SINGLE_QUOTE] 				= { 0                , '\'' ,  '"'  ,  0   },
		[KEY_BACKTICK] 					= { 0                , '`'  ,  '~'  ,  0   },
		[KEY_BACKSLASH] 				= { 0                , '\\' ,  '|'  ,  0   },
		[KEY_Z] 						= { CAPSLOCK         , 'z'  ,  'Z'  ,  0   },
		[KEY_X] 						= { CAPSLOCK         , 'x'  ,  'X'  ,  0   },
		[KEY_C] 						= { CAPSLOCK         , 'c'  ,  'C'  ,  0   },
		[KEY_V] 						= { CAPSLOCK         , 'v'  ,  'V'  ,  0   },
		[KEY_B] 						= { CAPSLOCK         , 'b'  ,  'B'  ,  0   },
		[KEY_N] 						= { CAPSLOCK         , 'n'  ,  'N'  ,  0   },
		[KEY_M] 						= { CAPSLOCK         , 'm'  ,  'M'  ,  0   },
		[KEY_COMMA] 					= { 0                , ','  ,  '<'  ,  0   },
		[KEY_DOT] 						= { 0                , '.'  ,  '>'  ,  0   },
		[KEY_SLASH] 					= { 0                , '/'  ,  '?'  ,  0   },
		[KEY_KEYPAD_WILDCARD] 			= { NO_MOD           , '*'                 },
		[KEY_SPACE] 					= { NO_MOD           , ' '                 },
		[KEY_KEYPAD_7] 					= { NUMLOCK | NO_MOD , '7'                 },
		[KEY_KEYPAD_8] 					= { NUMLOCK | NO_MOD , '8'                 },
		[KEY_KEYPAD_9] 					= { NUMLOCK | NO_MOD , '9'                 },
		[KEY_KEYPAD_DASH] 				= { NO_MOD           , '-'                 },
		[KEY_KEYPAD_4] 					= { NUMLOCK | NO_MOD , '4'                 },
		[KEY_KEYPAD_5] 					= { NUMLOCK | NO_MOD , '5'                 },
		[KEY_KEYPAD_6] 					= { NUMLOCK | NO_MOD , '6'                 },
		[KEY_KEYPAD_PLUS] 				= { NO_MOD           , '+'                 },
		[KEY_KEYPAD_1] 					= { NUMLOCK | NO_MOD , '1'                 },
		[KEY_KEYPAD_2] 					= { NUMLOCK | NO_MOD , '2'                 },
		[KEY_KEYPAD_3] 					= { NUMLOCK | NO_MOD , '3'                 },
		[KEY_KEYPAD_0] 					= { NUMLOCK | NO_MOD , '0'                 },
		[KEY_KEYPAD_DOT] 				= { NO_MOD           , '.'                 },
		[KEY_KEYPAD_ENTER] 				= { NO_MOD           , '\n'                },
		[KEY_KEYPAD_SLASH] 				= { NO_MOD           , '/'                 },
	};
	return keymap[kc];
}
