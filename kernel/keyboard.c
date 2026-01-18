/*
 * ============================================================================
 * Keyboard Driver Implementation
 * ============================================================================
 * PS/2 Keyboard driver using port-based I/O
 * Scans for keystrokes and converts scan codes to ASCII
 * ============================================================================
 */

#include "keyboard.h"
#include "screen.h"

/* Keyboard I/O Ports */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Status register bits */
#define KEYBOARD_OUTPUT_FULL 0x01

/* Modifier key states */
static bool shift_pressed = false;
static bool caps_lock = false;
static bool ctrl_pressed = false;

/* Scan code to ASCII lookup table (US QWERTY layout) */
static const char scancode_to_ascii[128] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',  0,    0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  0,    0,  'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/',  0,  '*',  0,  ' ',  0,    0,    0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,  '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/* Shifted characters */
static const char scancode_to_ascii_shift[128] = {
    0,    0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',  0,    0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',  0,    0,  'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0,  '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?',  0,  '*',  0,  ' ',  0,    0,    0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,  '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/* ============================================================================
 * Keyboard Functions
 * ============================================================================ */

/* Initialize keyboard */
void keyboard_init(void) {
    /* Flush the keyboard buffer */
    while (port_byte_in(KEYBOARD_STATUS_PORT) & KEYBOARD_OUTPUT_FULL) {
        port_byte_in(KEYBOARD_DATA_PORT);
    }
}

/* Check if a key is available */
bool keyboard_key_available(void) {
    return (port_byte_in(KEYBOARD_STATUS_PORT) & KEYBOARD_OUTPUT_FULL) != 0;
}

/* Read a character from keyboard (non-blocking, returns 0 if no key) */
char keyboard_read_char(void) {
    if (!keyboard_key_available()) {
        return 0;
    }
    
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);
    
    /* Check for key release (bit 7 set) */
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        /* Handle modifier key release */
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = false;
        } else if (scancode == KEY_CTRL) {
            ctrl_pressed = false;
        }
        
        return 0;  /* Key release, no character */
    }
    
    /* Handle modifier keys */
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = true;
        return 0;
    }
    
    if (scancode == KEY_CTRL) {
        ctrl_pressed = true;
        return 0;
    }
    
    if (scancode == KEY_CAPS) {
        caps_lock = !caps_lock;
        return 0;
    }
    
    /* Handle special keys */
    if (scancode == KEY_ENTER) {
        return '\n';
    }
    
    if (scancode == KEY_BACKSPACE) {
        return '\b';
    }
    
    if (scancode == KEY_TAB) {
        return '\t';
    }
    
    if (scancode == KEY_ESC) {
        return 27;  /* ESC character */
    }
    
    /* Convert scan code to ASCII */
    char c;
    if (shift_pressed) {
        c = scancode_to_ascii_shift[scancode];
    } else {
        c = scancode_to_ascii[scancode];
    }
    
    /* Apply caps lock to letters */
    if (caps_lock && c >= 'a' && c <= 'z') {
        c -= 32;  /* Convert to uppercase */
    } else if (caps_lock && c >= 'A' && c <= 'Z') {
        c += 32;  /* Convert to lowercase (caps + shift) */
    }
    
    return c;
}

/* Wait for a character (blocking) */
char keyboard_wait_char(void) {
    char c;
    while ((c = keyboard_read_char()) == 0) {
        /* Busy wait - no HLT since we don't have interrupts set up */
    }
    return c;
}

/* Read a line of input into buffer (with echo) */
int keyboard_read_line(char* buffer, int max_length) {
    int pos = 0;
    char c;
    
    while (pos < max_length - 1) {
        c = keyboard_wait_char();
        
        if (c == '\n') {
            buffer[pos] = '\0';
            screen_newline();
            return pos;
        }
        
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                screen_backspace();
            }
            continue;
        }
        
        /* Only accept printable characters */
        if (c >= 32 && c <= 126) {
            buffer[pos++] = c;
            screen_put_char(c);
        }
    }
    
    buffer[pos] = '\0';
    return pos;
}
