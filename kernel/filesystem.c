/*
 * ============================================================================
 * Simple File System Implementation
 * ============================================================================
 * A simple in-memory file system for demonstration purposes.
 * Files are stored in a fixed array with basic create/read/write/delete
 * functionality.
 * ============================================================================
 */

#include "filesystem.h"

/* File storage */
static FileEntry files[MAX_FILES];
static uint32_t file_count = 0;
static uint32_t time_counter = 0;

/* ============================================================================
 * Internal Functions
 * ============================================================================ */

/* Find a file by name, returns index or -1 if not found */
static int find_file(const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

/* Find an empty slot, returns index or -1 if full */
static int find_empty_slot(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            return i;
        }
    }
    return -1;
}

/* ============================================================================
 * File System Functions
 * ============================================================================ */

/* Initialize the file system */
void fs_init(void) {
    memset(files, 0, sizeof(files));
    file_count = 0;
    time_counter = 0;
    
    /* Create a welcome file */
    fs_create("welcome.txt");
    fs_write("welcome.txt", 
        "Welcome to MyOS!\n"
        "================\n\n"
        "This is a simple operating system built from scratch.\n"
        "Type 'help' to see available commands.\n\n"
        "Have fun exploring!\n");
    
    /* Create a readme file */
    fs_create("readme.txt");
    fs_write("readme.txt",
        "MyOS File System\n"
        "----------------\n\n"
        "Commands:\n"
        "  create <file>  - Create a new file\n"
        "  write <file> <text> - Write to file\n"
        "  read <file>    - Read file contents\n"
        "  list           - List all files\n"
        "  delete <file>  - Delete a file\n");
}

/* Create a new file */
int fs_create(const char* filename) {
    /* Validate filename */
    if (!filename || strlen(filename) == 0 || strlen(filename) >= MAX_FILENAME) {
        return FS_ERR_INVALID;
    }
    
    /* Check if file already exists */
    if (find_file(filename) >= 0) {
        return FS_ERR_EXISTS;
    }
    
    /* Find empty slot */
    int slot = find_empty_slot();
    if (slot < 0) {
        return FS_ERR_FULL;
    }
    
    /* Create the file */
    strcpy(files[slot].name, filename);
    files[slot].data[0] = '\0';
    files[slot].size = 0;
    files[slot].used = true;
    files[slot].created_time = ++time_counter;
    file_count++;
    
    return FS_SUCCESS;
}

/* Write content to a file (overwrites existing content) */
int fs_write(const char* filename, const char* content) {
    int idx = find_file(filename);
    if (idx < 0) {
        return FS_ERR_NOT_FOUND;
    }
    
    size_t len = strlen(content);
    if (len >= MAX_FILE_SIZE) {
        return FS_ERR_TOO_LARGE;
    }
    
    strcpy(files[idx].data, content);
    files[idx].size = len;
    
    return FS_SUCCESS;
}

/* Append content to a file */
int fs_append(const char* filename, const char* content) {
    int idx = find_file(filename);
    if (idx < 0) {
        return FS_ERR_NOT_FOUND;
    }
    
    size_t current_len = files[idx].size;
    size_t append_len = strlen(content);
    
    if (current_len + append_len >= MAX_FILE_SIZE) {
        return FS_ERR_TOO_LARGE;
    }
    
    strcpy(files[idx].data + current_len, content);
    files[idx].size = current_len + append_len;
    
    return FS_SUCCESS;
}

/* Read file contents into buffer */
int fs_read(const char* filename, char* buffer, size_t buffer_size) {
    int idx = find_file(filename);
    if (idx < 0) {
        return FS_ERR_NOT_FOUND;
    }
    
    size_t copy_len = files[idx].size;
    if (copy_len >= buffer_size) {
        copy_len = buffer_size - 1;
    }
    
    strncpy(buffer, files[idx].data, copy_len);
    buffer[copy_len] = '\0';
    
    return (int)files[idx].size;
}

/* Delete a file */
int fs_delete(const char* filename) {
    int idx = find_file(filename);
    if (idx < 0) {
        return FS_ERR_NOT_FOUND;
    }
    
    memset(&files[idx], 0, sizeof(FileEntry));
    file_count--;
    
    return FS_SUCCESS;
}

/* List all files */
int fs_list(char* buffer, size_t buffer_size) {
    char* ptr = buffer;
    char* end = buffer + buffer_size - 1;
    int count = 0;
    
    for (int i = 0; i < MAX_FILES && ptr < end; i++) {
        if (files[i].used) {
            /* Copy filename */
            const char* name = files[i].name;
            while (*name && ptr < end) {
                *ptr++ = *name++;
            }
            
            /* Add size info */
            if (ptr < end - 20) {
                *ptr++ = ' ';
                *ptr++ = '(';
                
                /* Convert size to string */
                char size_str[12];
                itoa((int)files[i].size, size_str, 10);
                const char* s = size_str;
                while (*s && ptr < end) {
                    *ptr++ = *s++;
                }
                
                if (ptr < end - 8) {
                    const char* bytes = " bytes)";
                    while (*bytes && ptr < end) {
                        *ptr++ = *bytes++;
                    }
                }
            }
            
            if (ptr < end) *ptr++ = '\n';
            count++;
        }
    }
    
    *ptr = '\0';
    return count;
}

/* Check if file exists */
int fs_exists(const char* filename) {
    return find_file(filename) >= 0 ? 1 : 0;
}

/* Get file size */
int fs_get_size(const char* filename) {
    int idx = find_file(filename);
    if (idx < 0) {
        return FS_ERR_NOT_FOUND;
    }
    return (int)files[idx].size;
}

/* Get total file count */
int fs_get_file_count(void) {
    return file_count;
}
