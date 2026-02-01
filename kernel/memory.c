/*
 * ============================================================================
 * Memory Allocator Implementation
 * ============================================================================
 * A simple first-fit heap allocator for MyOS.
 *
 * Features:
 * - First-fit allocation strategy
 * - Block coalescing on free
 * - 8-byte alignment
 * - Magic number validation
 * ============================================================================
 */

#include "memory.h"
#include "screen.h"

/*
 * Heap memory starts after the kernel.
 * Defined in linker.ld
 */
extern uint32_t _kernel_end;

/* Head of the free list */
static BlockHeader *heap_start = NULL;
static bool heap_initialized = false;

/* Statistics */
static size_t total_allocated = 0;
static size_t total_freed = 0;
static size_t num_allocations = 0;

/* ============================================================================
 * Internal Functions
 * ============================================================================
 */

/* Align size to BLOCK_ALIGN bytes */
static size_t align_size(size_t size) {
  return (size + BLOCK_ALIGN - 1) & ~(BLOCK_ALIGN - 1);
}

/* Get pointer to user data from block header */
static void *header_to_data(BlockHeader *header) {
  return (void *)((uint8_t *)header + HEADER_SIZE);
}

/* Get block header from user data pointer */
static BlockHeader *data_to_header(void *ptr) {
  return (BlockHeader *)((uint8_t *)ptr - HEADER_SIZE);
}

/* Validate a block header */
static bool is_valid_block(BlockHeader *block) {
  if (!block)
    return false;
  if (block->magic != BLOCK_MAGIC)
    return false;
  return true;
}

/* Split a block if it's large enough */
static void split_block(BlockHeader *block, size_t size) {
  size_t total_needed = HEADER_SIZE + size;
  size_t remaining = block->size - total_needed;

  /* Only split if remaining space can hold a useful block */
  if (remaining >= HEADER_SIZE + MIN_BLOCK_SIZE) {
    BlockHeader *new_block = (BlockHeader *)((uint8_t *)block + total_needed);
    new_block->size = remaining;
    new_block->is_free = 1;
    new_block->magic = BLOCK_MAGIC;
    new_block->next = block->next;
    new_block->prev = block;

    if (block->next) {
      block->next->prev = new_block;
    }

    block->next = new_block;
    block->size = total_needed;
  }
}

/* Coalesce adjacent free blocks */
static void coalesce(BlockHeader *block) {
  /* Coalesce with next block if free */
  if (block->next && block->next->is_free) {
    block->size += block->next->size;
    block->next = block->next->next;
    if (block->next) {
      block->next->prev = block;
    }
  }

  /* Coalesce with previous block if free */
  if (block->prev && block->prev->is_free) {
    block->prev->size += block->size;
    block->prev->next = block->next;
    if (block->next) {
      block->next->prev = block->prev;
    }
  }
}

/* ============================================================================
 * Public Functions
 * ============================================================================
 */

/* Initialize the heap */
void memory_init(void) {
  if (heap_initialized)
    return;

  /* Calculate heap start address (aligned) */
  uint32_t kernel_end_addr = (uint32_t)&_kernel_end;
  uint32_t heap_start_addr =
      (kernel_end_addr + BLOCK_ALIGN - 1) & ~(BLOCK_ALIGN - 1);

  /* Create initial free block spanning entire heap */
  heap_start = (BlockHeader *)heap_start_addr;
  heap_start->size = HEAP_SIZE;
  heap_start->is_free = 1;
  heap_start->next = NULL;
  heap_start->prev = NULL;
  heap_start->magic = BLOCK_MAGIC;

  heap_initialized = true;
  total_allocated = 0;
  total_freed = 0;
  num_allocations = 0;

  screen_print("Heap initialized at: ");
  char buf[32];
  itoa(heap_start_addr, buf, 16);
  screen_print("0x");
  screen_print(buf);
  screen_print("\n");
}

/* Allocate memory */
void *malloc(size_t size) {
  if (!heap_initialized) {
    memory_init();
  }

  if (size == 0)
    return NULL;

  /* Align the requested size */
  size = align_size(size);
  if (size < MIN_BLOCK_SIZE) {
    size = MIN_BLOCK_SIZE;
  }

  /* First-fit search */
  BlockHeader *current = heap_start;
  while (current) {
    if (current->is_free && current->size >= HEADER_SIZE + size) {
      /* Found a suitable block */
      split_block(current, size);
      current->is_free = 0;

      /* Update statistics */
      total_allocated += current->size;
      num_allocations++;

      return header_to_data(current);
    }
    current = current->next;
  }

  /* No suitable block found */
  return NULL;
}

/* Free memory */
void free(void *ptr) {
  if (!ptr)
    return;

  BlockHeader *block = data_to_header(ptr);

  /* Validate block */
  if (!is_valid_block(block)) {
    screen_print_color("ERROR: Invalid free() - bad pointer!\n", ERROR_COLOR);
    return;
  }

  if (block->is_free) {
    screen_print_color("ERROR: Double free detected!\n", ERROR_COLOR);
    return;
  }

  /* Mark as free */
  block->is_free = 1;
  total_freed += block->size;

  /* Coalesce with adjacent free blocks */
  coalesce(block);
}

/* Allocate and zero memory */
void *calloc(size_t num, size_t size) {
  size_t total = num * size;
  void *ptr = malloc(total);
  if (ptr) {
    memset(ptr, 0, total);
  }
  return ptr;
}

/* Reallocate memory */
void *realloc(void *ptr, size_t new_size) {
  if (!ptr) {
    return malloc(new_size);
  }

  if (new_size == 0) {
    free(ptr);
    return NULL;
  }

  BlockHeader *block = data_to_header(ptr);
  if (!is_valid_block(block)) {
    return NULL;
  }

  size_t old_size = block->size - HEADER_SIZE;

  /* If new size fits in current block, just return */
  if (new_size <= old_size) {
    return ptr;
  }

  /* Allocate new block and copy */
  void *new_ptr = malloc(new_size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
  }

  return new_ptr;
}

/* Get total free memory */
size_t memory_get_free(void) {
  size_t free_mem = 0;
  BlockHeader *current = heap_start;

  while (current) {
    if (current->is_free) {
      free_mem += current->size - HEADER_SIZE;
    }
    current = current->next;
  }

  return free_mem;
}

/* Get total used memory */
size_t memory_get_used(void) {
  size_t used_mem = 0;
  BlockHeader *current = heap_start;

  while (current) {
    if (!current->is_free) {
      used_mem += current->size - HEADER_SIZE;
    }
    current = current->next;
  }

  return used_mem;
}

/* Dump heap status for debugging */
void memory_dump(void) {
  screen_print_color("\n=== Heap Memory Status ===\n", INFO_COLOR);

  char buf[64];

  screen_print("Total heap size: ");
  itoa(HEAP_SIZE / 1024, buf, 10);
  screen_print(buf);
  screen_print(" KB\n");

  screen_print("Used memory:     ");
  itoa(memory_get_used() / 1024, buf, 10);
  screen_print(buf);
  screen_print(" KB\n");

  screen_print("Free memory:     ");
  itoa(memory_get_free() / 1024, buf, 10);
  screen_print(buf);
  screen_print(" KB\n");

  screen_print("Allocations:     ");
  itoa(num_allocations, buf, 10);
  screen_print(buf);
  screen_print("\n");

  /* Show block list */
  screen_print_color("\nBlock list:\n", INFO_COLOR);
  BlockHeader *current = heap_start;
  int block_num = 0;

  while (current && block_num < 10) { /* Limit display */
    screen_print("  [");
    itoa(block_num, buf, 10);
    screen_print(buf);
    screen_print("] ");

    itoa(current->size / 1024, buf, 10);
    screen_print(buf);
    screen_print("KB ");

    if (current->is_free) {
      screen_print_color("FREE", PROMPT_COLOR);
    } else {
      screen_print_color("USED", ERROR_COLOR);
    }
    screen_print("\n");

    current = current->next;
    block_num++;
  }

  if (current) {
    screen_print("  ... more blocks ...\n");
  }

  screen_print("\n");
}
