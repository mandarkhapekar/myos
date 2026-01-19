/*
 * Shell Implementation - Command line interface
 */

#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "filesystem.h"
#include "memory.h"
#include "math.h"
#include "ata.h"

static char command_buffer[MAX_COMMAND_LENGTH];

/* Skip whitespace and return pointer to next word */
static char* skip_spaces(char* str) {
    while (*str == ' ' || *str == '\t') str++;
    return str;
}

/* Get next word, null-terminate it, return pointer after it */
static char* get_word(char* str, char** word) {
    str = skip_spaces(str);
    if (*str == '\0') {
        *word = NULL;
        return str;
    }
    *word = str;
    while (*str && *str != ' ' && *str != '\t') str++;
    if (*str) *str++ = '\0';
    return str;
}

/* Command handlers */
static void cmd_help(void) {
    screen_print_color("\n=== MyOS Commands ===\n", HIGHLIGHT_COLOR);
    screen_print("  help              - Show this help\n");
    screen_print("  clear             - Clear the screen\n");
    screen_print("  about             - About MyOS\n");
    screen_print("  mem               - Show memory status\n");
    screen_print("  math              - Test math library\n");
    screen_print("  disk              - Test disk reading\n");
    screen_print("  list              - List all files\n");
    screen_print("  create <file>     - Create a new file\n");
    screen_print("  read <file>       - Read file contents\n");
    screen_print("  write <file> <txt>- Write text to file\n");
    screen_print("  delete <file>     - Delete a file\n\n");
}

static void cmd_about(void) {
    screen_print_color("\n*** MyOS v2.0-dev ***\n", HIGHLIGHT_COLOR);
    screen_print("A primitive OS with native AI inference!\n");
    screen_print("Features: Shell, Keyboard, VGA, FileSystem, Memory Manager\n\n");
}

static void cmd_mem(void) {
    memory_dump();
}

/* Helper to print float (simple version) */
static void print_float(float f) {
    char buf[16];
    if (f < 0) { screen_print("-"); f = -f; }
    int whole = (int)f;
    int frac = (int)((f - whole) * 1000);
    itoa(whole, buf, 10);
    screen_print(buf);
    screen_print(".");
    if (frac < 100) screen_print("0");
    if (frac < 10) screen_print("0");
    itoa(frac, buf, 10);
    screen_print(buf);
}

static void cmd_math(void) {
    screen_print_color("\n=== Math Library Test ===\n", HIGHLIGHT_COLOR);
    
    screen_print("expf(1.0) = "); print_float(expf(1.0f)); screen_print(" (expect 2.718)\n");
    screen_print("expf(0.0) = "); print_float(expf(0.0f)); screen_print(" (expect 1.000)\n");
    screen_print("logf(2.718) = "); print_float(logf(2.718f)); screen_print(" (expect 1.000)\n");
    screen_print("sqrtf(4.0) = "); print_float(sqrtf(4.0f)); screen_print(" (expect 2.000)\n");
    screen_print("sqrtf(2.0) = "); print_float(sqrtf(2.0f)); screen_print(" (expect 1.414)\n");
    screen_print("powf(2,10) = "); print_float(powf(2.0f, 10.0f)); screen_print(" (expect 1024)\n");
    screen_print("tanhf(0.0) = "); print_float(tanhf(0.0f)); screen_print(" (expect 0.000)\n");
    screen_print("tanhf(1.0) = "); print_float(tanhf(1.0f)); screen_print(" (expect 0.761)\n");
    
    screen_print_color("\nMath library ready for LLM inference!\n\n", INFO_COLOR);
}

static void cmd_disk(void) {
    screen_print_color("\n=== ATA Disk Test ===\n", HIGHLIGHT_COLOR);
    
    uint8_t* buffer = (uint8_t*)malloc(512);
    if (!buffer) {
        screen_print_color("Error: Could not allocate buffer\n", ERROR_COLOR);
        return;
    }
    
    screen_print("Reading sector 0 (bootloader)...\n");
    int result = ata_read_sectors(0, 1, buffer);
    
    if (result == ATA_SUCCESS) {
        screen_print_color("Success! ", INFO_COLOR);
        screen_print("First 16 bytes: ");
        char hex[4];
        for (int i = 0; i < 16; i++) {
            uint8_t b = buffer[i];
            hex[0] = "0123456789ABCDEF"[b >> 4];
            hex[1] = "0123456789ABCDEF"[b & 0xF];
            hex[2] = ' ';
            hex[3] = 0;
            screen_print(hex);
        }
        screen_print("\n");
        
        /* Check boot signature */
        if (buffer[510] == 0x55 && buffer[511] == 0xAA) {
            screen_print_color("Boot signature (0x55AA) found!\n", INFO_COLOR);
        }
    } else {
        screen_print_color("Error reading disk!\n", ERROR_COLOR);
    }
    
    free(buffer);
    screen_print("\n");
}

static void cmd_clear(void) {
    screen_clear();
}

static void cmd_list(void) {
    char buffer[512];
    int count = fs_list(buffer, sizeof(buffer));
    if (count > 0) {
        screen_print_color("\nFiles:\n", INFO_COLOR);
        screen_print(buffer);
    } else {
        screen_print("No files found.\n");
    }
}

static void cmd_create(char* args) {
    char* filename;
    get_word(args, &filename);
    if (!filename) {
        screen_print_color("Usage: create <filename>\n", ERROR_COLOR);
        return;
    }
    int result = fs_create(filename);
    if (result == FS_SUCCESS) {
        screen_print_color("File created: ", INFO_COLOR);
        screen_print(filename);
        screen_print("\n");
    } else if (result == FS_ERR_EXISTS) {
        screen_print_color("Error: File already exists\n", ERROR_COLOR);
    } else {
        screen_print_color("Error: Could not create file\n", ERROR_COLOR);
    }
}

static void cmd_read(char* args) {
    char* filename;
    get_word(args, &filename);
    if (!filename) {
        screen_print_color("Usage: read <filename>\n", ERROR_COLOR);
        return;
    }
    char buffer[MAX_FILE_SIZE];
    int result = fs_read(filename, buffer, sizeof(buffer));
    if (result >= 0) {
        screen_print_color("\n--- ", INFO_COLOR);
        screen_print(filename);
        screen_print_color(" ---\n", INFO_COLOR);
        screen_print(buffer);
        screen_print("\n");
    } else {
        screen_print_color("Error: File not found\n", ERROR_COLOR);
    }
}

static void cmd_write(char* args) {
    char* filename;
    args = get_word(args, &filename);
    if (!filename) {
        screen_print_color("Usage: write <filename> <content>\n", ERROR_COLOR);
        return;
    }
    char* content = skip_spaces(args);
    if (!*content) {
        screen_print_color("Usage: write <filename> <content>\n", ERROR_COLOR);
        return;
    }
    if (!fs_exists(filename)) {
        fs_create(filename);
    }
    int result = fs_write(filename, content);
    if (result == FS_SUCCESS) {
        screen_print_color("Written to: ", INFO_COLOR);
        screen_print(filename);
        screen_print("\n");
    } else {
        screen_print_color("Error: Could not write\n", ERROR_COLOR);
    }
}

static void cmd_delete(char* args) {
    char* filename;
    get_word(args, &filename);
    if (!filename) {
        screen_print_color("Usage: delete <filename>\n", ERROR_COLOR);
        return;
    }
    int result = fs_delete(filename);
    if (result == FS_SUCCESS) {
        screen_print_color("Deleted: ", INFO_COLOR);
        screen_print(filename);
        screen_print("\n");
    } else {
        screen_print_color("Error: File not found\n", ERROR_COLOR);
    }
}

/* Initialize shell */
void shell_init(void) {
    memset(command_buffer, 0, sizeof(command_buffer));
}

/* Print prompt */
void shell_print_prompt(void) {
    screen_print_color("myos> ", PROMPT_COLOR);
}

/* Execute a command */
void shell_execute(const char* input) {
    char cmd_copy[MAX_COMMAND_LENGTH];
    strncpy(cmd_copy, input, MAX_COMMAND_LENGTH - 1);
    cmd_copy[MAX_COMMAND_LENGTH - 1] = '\0';
    
    char* cmd;
    char* rest = get_word(cmd_copy, &cmd);
    
    if (!cmd || !*cmd) return;
    
    if (strcmp(cmd, "help") == 0) cmd_help();
    else if (strcmp(cmd, "clear") == 0) cmd_clear();
    else if (strcmp(cmd, "about") == 0) cmd_about();
    else if (strcmp(cmd, "mem") == 0) cmd_mem();
    else if (strcmp(cmd, "math") == 0) cmd_math();
    else if (strcmp(cmd, "disk") == 0) cmd_disk();
    else if (strcmp(cmd, "list") == 0) cmd_list();
    else if (strcmp(cmd, "create") == 0) cmd_create(rest);
    else if (strcmp(cmd, "read") == 0) cmd_read(rest);
    else if (strcmp(cmd, "write") == 0) cmd_write(rest);
    else if (strcmp(cmd, "delete") == 0) cmd_delete(rest);
    else {
        screen_print_color("Unknown command: ", ERROR_COLOR);
        screen_print(cmd);
        screen_print("\nType 'help' for commands.\n");
    }
}

/* Main shell loop */
void shell_run(void) {
    shell_print_prompt();
    
    while (1) {
        int len = keyboard_read_line(command_buffer, MAX_COMMAND_LENGTH);
        if (len > 0) {
            shell_execute(command_buffer);
        }
        shell_print_prompt();
    }
}
