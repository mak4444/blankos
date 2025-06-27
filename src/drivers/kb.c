// Keyboard driver
// Author: xamidev
// Licensed under the Unlicense. See the repo below.
// https://github.com/xamidev/blankos

#include "../kernel/io.h"
#include "../libc/stdio.h"
#include "../kernel/system.h"
#include "kb.h"

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
    'm', ',', '.', '/',   0,				/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char kbdus_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
    '(', ')', '_', '+', '\b',	/* Backspace */
    '\t',			/* Tab */
    'Q', 'W', 'E', 'R',	/* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
    '"', '~',   0,		/* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
    'M', '<', '>', '?',   0,				/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static unsigned int keyboard_buffer_start = 0;
static unsigned int keyboard_buffer_end = 0;
int shift_pressed = 0;

static char sc_keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static unsigned int sc_keyboard_buffer_start = 0;
static unsigned int sc_keyboard_buffer_end = 0;

void keyboard_handler()
{
	unsigned char scancode;

	scancode = inb(0x60);

	sc_keyboard_buffer[sc_keyboard_buffer_end] = scancode;
	sc_keyboard_buffer_end = (sc_keyboard_buffer_end+1) % KEYBOARD_BUFFER_SIZE;

	if (scancode & 0x80)
	{
		if (scancode == LEFT_SHIFT_RELEASED || scancode == RIGHT_SHIFT_RELEASED) {
			shift_pressed = 0;
		}
	}
	else 
	{
		if (scancode == LEFT_SHIFT_PRESSED || scancode == RIGHT_SHIFT_PRESSED) {
			shift_pressed = 1;
		} else {
			char c;
			if (shift_pressed) {
				c = kbdus_shift[scancode];
			} else {
				c = kbdus[scancode];
			}
		
			if (c)
			{
				keyboard_buffer[keyboard_buffer_end] = c;
				keyboard_buffer_end = (keyboard_buffer_end+1) % KEYBOARD_BUFFER_SIZE;
			}
		}
	}
}

void keyboard_install()
{
	irq_install_handler(1, keyboard_handler);
	printf("[keyboard] installed irq handler\n");
}

char keyboard_getchar()
{       draw_cursor(white);
	while (!keyboard_has_input());

	char c = keyboard_buffer[keyboard_buffer_start];
	keyboard_buffer_start = (keyboard_buffer_start+1) % KEYBOARD_BUFFER_SIZE;
	erase_cursor();
	return c;
}

int keyboard_has_input()
{
	if ( (global_ticks & 0x1f) == 0)
	{
		draw_cursor(white);
	} else if ( (global_ticks & 0x1f) == 0x10 ) {
		erase_cursor();
	}

	return keyboard_buffer_start != keyboard_buffer_end;
}

char keyboard_getchar_non_blocking()
{       
	if (keyboard_has_input())
	{
		char c = keyboard_buffer[keyboard_buffer_start];
		keyboard_buffer_start = (keyboard_buffer_start+1)%KEYBOARD_BUFFER_SIZE;
		return c;
	}
	return 0;
}

char sc_keyboard_getchar()
{	while (sc_keyboard_buffer_start == sc_keyboard_buffer_end);

	char c = sc_keyboard_buffer[sc_keyboard_buffer_start];
	sc_keyboard_buffer_start = (sc_keyboard_buffer_start+1) % KEYBOARD_BUFFER_SIZE;
	return c;
}

int sc_keyboard_has_input()
{
	return sc_keyboard_buffer_start != sc_keyboard_buffer_end;
}

