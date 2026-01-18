/*
 * ============================================================================
 * Simple File System Header
 * ============================================================================
 * In-memory file system for demonstration
 * ============================================================================
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "kernel.h"

/* File System Constants */
#define MAX_FILES         32
#define MAX_FILENAME      32
#define MAX_FILE_SIZE     1024

/* File Entry */
typedef struct {
    char     name[MAX_FILENAME];
    char     data[MAX_FILE_SIZE];
    uint32_t size;
    bool     used;
    uint32_t created_time;
} FileEntry;

/* File System Functions */
void fs_init(void);
int  fs_create(const char* filename);
int  fs_write(const char* filename, const char* content);
int  fs_append(const char* filename, const char* content);
int  fs_read(const char* filename, char* buffer, size_t buffer_size);
int  fs_delete(const char* filename);
int  fs_list(char* buffer, size_t buffer_size);
int  fs_exists(const char* filename);
int  fs_get_size(const char* filename);
int  fs_get_file_count(void);

/* Error codes */
#define FS_SUCCESS          0
#define FS_ERR_NOT_FOUND   -1
#define FS_ERR_EXISTS      -2
#define FS_ERR_FULL        -3
#define FS_ERR_TOO_LARGE   -4
#define FS_ERR_INVALID     -5

#endif /* FILESYSTEM_H */
