/*
 * ============================================================================
 * Screen Driver Header
 * ============================================================================
 * VGA text mode display functions
 * ============================================================================
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "kernel.h"

/* VGA Text Mode Constants */
#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

/* Color Attributes */
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GREY    0x7
#define COLOR_DARK_GREY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

/* Create color attribute byte */
#define MAKE_COLOR(fg, bg) ((bg << 4) | fg)

/* Default colors */
#define DEFAULT_COLOR MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK)
#define PROMPT_COLOR MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK)
#define ERROR_COLOR MAKE_COLOR(COLOR_LIGHT_RED, COLOR_BLACK)
#define INFO_COLOR MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK)
#define HIGHLIGHT_COLOR MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK)

/* Screen Functions */
void screen_init(void);
void screen_clear(void);
void screen_put_char(char c);
void screen_put_char_color(char c, uint8_t color);
void screen_print(const char* str);
void screen_print_color(const char* str, uint8_t color);
void screen_print_line(const char* str);
void screen_print_int(int value);
void screen_set_cursor(int row, int col);
void screen_get_cursor(int* row, int* col);
void screen_scroll(void);
void screen_backspace(void);
void screen_newline(void);

#endif /* SCREEN_H */
