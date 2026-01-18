/*
 * ============================================================================
 * Screen Driver Implementation
 * ============================================================================
 * VGA text mode display driver
 * Writes directly to video memory at 0xB8000
 * ============================================================================
 */

#include "screen.h"

/* VGA I/O Ports */
#define VGA_CTRL_REG 0x3D4
#define VGA_DATA_REG 0x3D5

/* Current cursor position */
static int cursor_row = 0;
static int cursor_col = 0;

/* Current color attribute */
static uint8_t current_color = DEFAULT_COLOR;

/* Video memory pointer */
static volatile uint16_t* video_memory = (volatile uint16_t*)VIDEO_MEMORY;

/* ============================================================================
 * Internal Functions
 * ============================================================================ */

/* Update hardware cursor position */
static void update_cursor(void) {
    uint16_t pos = cursor_row * SCREEN_WIDTH + cursor_col;
    
    port_byte_out(VGA_CTRL_REG, 14);
    port_byte_out(VGA_DATA_REG, (pos >> 8) & 0xFF);
    port_byte_out(VGA_CTRL_REG, 15);
    port_byte_out(VGA_DATA_REG, pos & 0xFF);
}

/* Get offset in video memory */
static int get_offset(int row, int col) {
    return row * SCREEN_WIDTH + col;
}

/* ============================================================================
 * Screen Functions
 * ============================================================================ */

/* Initialize the screen */
void screen_init(void) {
    screen_clear();
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

/* Clear the entire screen */
void screen_clear(void) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video_memory[i] = (DEFAULT_COLOR << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

/* Scroll the screen up by one line */
void screen_scroll(void) {
    /* Move all lines up by one */
    for (int i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
        video_memory[i] = video_memory[i + SCREEN_WIDTH];
    }
    
    /* Clear the last line */
    int start = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH;
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        video_memory[start + i] = (DEFAULT_COLOR << 8) | ' ';
    }
    
    cursor_row = SCREEN_HEIGHT - 1;
}

/* Print a single character with color */
void screen_put_char_color(char c, uint8_t color) {
    if (c == '\n') {
        screen_newline();
        return;
    }
    
    if (c == '\r') {
        cursor_col = 0;
        update_cursor();
        return;
    }
    
    if (c == '\t') {
        /* Tab to next 4-column boundary */
        cursor_col = (cursor_col + 4) & ~3;
        if (cursor_col >= SCREEN_WIDTH) {
            screen_newline();
        }
        update_cursor();
        return;
    }
    
    if (c == '\b') {
        screen_backspace();
        return;
    }
    
    /* Regular character */
    int offset = get_offset(cursor_row, cursor_col);
    video_memory[offset] = (color << 8) | (uint8_t)c;
    
    cursor_col++;
    if (cursor_col >= SCREEN_WIDTH) {
        screen_newline();
    }
    
    update_cursor();
}

/* Print a single character with default color */
void screen_put_char(char c) {
    screen_put_char_color(c, current_color);
}

/* Handle newline */
void screen_newline(void) {
    cursor_col = 0;
    cursor_row++;
    
    if (cursor_row >= SCREEN_HEIGHT) {
        screen_scroll();
    }
    
    update_cursor();
}

/* Handle backspace */
void screen_backspace(void) {
    if (cursor_col > 0) {
        cursor_col--;
    } else if (cursor_row > 0) {
        cursor_row--;
        cursor_col = SCREEN_WIDTH - 1;
    }
    
    int offset = get_offset(cursor_row, cursor_col);
    video_memory[offset] = (current_color << 8) | ' ';
    update_cursor();
}

/* Print a string with color */
void screen_print_color(const char* str, uint8_t color) {
    while (*str) {
        screen_put_char_color(*str++, color);
    }
}

/* Print a string with default color */
void screen_print(const char* str) {
    screen_print_color(str, current_color);
}

/* Print a string followed by newline */
void screen_print_line(const char* str) {
    screen_print(str);
    screen_newline();
}

/* Print an integer */
void screen_print_int(int value) {
    char buffer[12];
    itoa(value, buffer, 10);
    screen_print(buffer);
}

/* Set cursor position */
void screen_set_cursor(int row, int col) {
    if (row >= 0 && row < SCREEN_HEIGHT && col >= 0 && col < SCREEN_WIDTH) {
        cursor_row = row;
        cursor_col = col;
        update_cursor();
    }
}

/* Get cursor position */
void screen_get_cursor(int* row, int* col) {
    if (row) *row = cursor_row;
    if (col) *col = cursor_col;
}
