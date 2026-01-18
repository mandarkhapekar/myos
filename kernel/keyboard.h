/*
 * ============================================================================
 * Keyboard Driver Header
 * ============================================================================
 * PS/2 Keyboard input handling
 * ============================================================================
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

/* Special key codes */
#define KEY_BACKSPACE 0x0E
#define KEY_ENTER     0x1C
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_CAPS      0x3A
#define KEY_CTRL      0x1D
#define KEY_ALT       0x38
#define KEY_SPACE     0x39
#define KEY_TAB       0x0F
#define KEY_ESC       0x01

#define KEY_UP        0x48
#define KEY_DOWN      0x50
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D

/* Keyboard Functions */
void keyboard_init(void);
char keyboard_read_char(void);
char keyboard_wait_char(void);
int keyboard_read_line(char* buffer, int max_length);
bool keyboard_key_available(void);

#endif /* KEYBOARD_H */
