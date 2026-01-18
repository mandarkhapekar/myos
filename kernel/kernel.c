/*
 * MyOS Kernel - Main entry point
 */

#include "kernel.h"
#include "screen.h"
#include "keyboard.h"
#include "filesystem.h"
#include "shell.h"

/* Print welcome banner */
static void print_banner(void) {
    screen_print_color("\n", DEFAULT_COLOR);
    screen_print_color("  __  __        ___  ____  \n", HIGHLIGHT_COLOR);
    screen_print_color(" |  \\/  |_   _ / _ \\/ ___| \n", HIGHLIGHT_COLOR);
    screen_print_color(" | |\\/| | | | | | | \\___ \\ \n", HIGHLIGHT_COLOR);
    screen_print_color(" | |  | | |_| | |_| |___) |\n", HIGHLIGHT_COLOR);
    screen_print_color(" |_|  |_|\\__, |\\___/|____/ \n", HIGHLIGHT_COLOR);
    screen_print_color("         |___/             \n", HIGHLIGHT_COLOR);
    screen_print_color("\n", DEFAULT_COLOR);
    screen_print_color(" Welcome to MyOS v1.0!\n", INFO_COLOR);
    screen_print_color(" A primitive operating system built from scratch.\n\n", DEFAULT_COLOR);
    screen_print(" Type 'help' to see available commands.\n");
    screen_print(" Type 'read welcome.txt' for more info.\n\n");
}

/* Main kernel function - called from kernel_entry.asm */
void kernel_main(void) {
    /* Initialize subsystems */
    screen_init();
    keyboard_init();
    fs_init();
    shell_init();
    
    /* Print welcome banner */
    print_banner();
    
    /* Start the shell */
    shell_run();
    
    /* Should never reach here */
    screen_print_color("\nKernel halted.\n", ERROR_COLOR);
    while (1) {
        __asm__ volatile ("hlt");
    }
}
