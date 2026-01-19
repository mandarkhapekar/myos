/*
 * ============================================================================
 * Memory Allocator Header
 * ============================================================================
 * Simple heap allocator for MyOS
 * ============================================================================
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"

/* Heap configuration */
#define HEAP_SIZE       (8 * 1024 * 1024)   /* 8 MB heap */
#define HEAP_START      0x200000             /* Start at 2 MB mark */
#define BLOCK_ALIGN     8                    /* 8-byte alignment */
#define MIN_BLOCK_SIZE  16                   /* Minimum allocation */

/* Memory block header */
typedef struct BlockHeader {
    uint32_t size;              /* Size of this block (including header) */
    uint32_t is_free;           /* 1 if free, 0 if allocated */
    struct BlockHeader* next;   /* Next block in list */
    struct BlockHeader* prev;   /* Previous block in list */
    uint32_t magic;             /* Magic number for validation */
} BlockHeader;

#define BLOCK_MAGIC     0xDEADBEEF
#define HEADER_SIZE     sizeof(BlockHeader)

/* Memory functions */
void  memory_init(void);
void* malloc(size_t size);
void  free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t new_size);

/* Debug functions */
void  memory_dump(void);
size_t memory_get_free(void);
size_t memory_get_used(void);

#endif /* MEMORY_H */
