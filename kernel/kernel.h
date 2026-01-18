/*
 * ============================================================================
 * MyOS Kernel Header
 * ============================================================================
 * Core type definitions and function declarations
 * ============================================================================
 */

#ifndef KERNEL_H
#define KERNEL_H

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef uint32_t size_t;

#define NULL ((void*)0)
#define true 1
#define false 0
typedef int bool;

/* ============================================================================
 * I/O Port Functions (defined in kernel_entry.asm)
 * ============================================================================ */

extern uint8_t port_byte_in(uint16_t port);
extern void port_byte_out(uint16_t port, uint8_t data);
extern uint16_t port_word_in(uint16_t port);
extern void port_word_out(uint16_t port, uint16_t data);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/* String length */
static inline size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

/* String compare */
static inline int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/* String compare (n characters) */
static inline int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/* String copy */
static inline char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

/* String copy (n characters) */
static inline char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n && (*d++ = *src++)) n--;
    while (n--) *d++ = '\0';
    return dest;
}

/* Memory set */
static inline void* memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    while (num--) *p++ = (uint8_t)value;
    return ptr;
}

/* Memory copy */
static inline void* memcpy(void* dest, const void* src, size_t num) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (num--) *d++ = *s++;
    return dest;
}

/* Convert integer to string */
static inline void itoa(int value, char* str, int base) {
    char* p = str;
    char* p1, *p2;
    unsigned int uvalue = value;
    int digit;
    
    if (base == 10 && value < 0) {
        *p++ = '-';
        uvalue = -value;
    }
    
    p1 = p;
    
    do {
        digit = uvalue % base;
        *p++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        uvalue /= base;
    } while (uvalue);
    
    *p = '\0';
    
    /* Reverse the string */
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }
}

#endif /* KERNEL_H */
