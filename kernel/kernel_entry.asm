; ============================================================================
; Kernel Entry Point
; ============================================================================
; This is the entry point for the kernel. It:
; 1. Calls the main kernel function (written in C)
; 2. Provides low-level I/O port access functions
; ============================================================================

[bits 32]
[extern kernel_main]        ; Declare external C function

section .text
    global _start
    global port_byte_in
    global port_byte_out
    global port_word_in
    global port_word_out

; ============================================================================
; Entry Point
; ============================================================================

_start:
    call kernel_main        ; Call C kernel
    jmp $                   ; Hang if kernel returns

; ============================================================================
; I/O Port Functions
; ============================================================================

; Read a byte from an I/O port
; uint8_t port_byte_in(uint16_t port)
port_byte_in:
    mov edx, [esp + 4]      ; Port number
    in al, dx               ; Read byte from port
    ret

; Write a byte to an I/O port
; void port_byte_out(uint16_t port, uint8_t data)
port_byte_out:
    mov edx, [esp + 4]      ; Port number
    mov al, [esp + 8]       ; Data to write
    out dx, al              ; Write byte to port
    ret

; Read a word from an I/O port
; uint16_t port_word_in(uint16_t port)
port_word_in:
    mov edx, [esp + 4]      ; Port number
    in ax, dx               ; Read word from port
    ret

; Write a word to an I/O port
; void port_word_out(uint16_t port, uint16_t data)
port_word_out:
    mov edx, [esp + 4]      ; Port number
    mov ax, [esp + 8]       ; Data to write
    out dx, ax              ; Write word to port
    ret
