; ============================================================================
; MyOS Bootloader
; ============================================================================
; This bootloader:
; 1. Is loaded by BIOS at address 0x7C00
; 2. Switches from 16-bit real mode to 32-bit protected mode
; 3. Loads the kernel from disk
; 4. Jumps to the kernel
; ============================================================================

[bits 16]
[org 0x7C00]

KERNEL_OFFSET equ 0x1000    ; Memory offset where we'll load the kernel

; Entry point
start:
    ; Set up segment registers and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack grows downward from bootloader

    mov [BOOT_DRIVE], dl    ; BIOS stores boot drive in DL

    ; Print welcome message
    mov si, MSG_BOOT
    call print_string_16

    ; Load kernel from disk
    call load_kernel

    ; Switch to 32-bit protected mode
    call switch_to_pm

    jmp $                   ; Should never reach here

; ============================================================================
; 16-bit Functions (Real Mode)
; ============================================================================

; Print null-terminated string in SI
print_string_16:
    pusha
    mov ah, 0x0E            ; BIOS teletype function
.loop:
    lodsb                   ; Load byte from SI into AL
    cmp al, 0
    je .done
    int 0x10                ; BIOS video interrupt
    jmp .loop
.done:
    popa
    ret

; Load kernel from disk
load_kernel:
    mov si, MSG_LOAD
    call print_string_16

    mov bx, KERNEL_OFFSET   ; Load to ES:BX = 0x0000:0x1000
    mov dh, 30              ; Read 30 sectors (enough for our kernel)
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret

; Load DH sectors from drive DL into ES:BX
disk_load:
    pusha
    push dx

    mov ah, 0x02            ; BIOS read sector function
    mov al, dh              ; Number of sectors to read
    mov ch, 0               ; Cylinder 0
    mov dh, 0               ; Head 0
    mov cl, 2               ; Start from sector 2 (sector 1 is bootloader)

    int 0x13                ; BIOS disk interrupt
    jc disk_error           ; Jump if carry flag set (error)

    pop dx
    cmp al, dh              ; Compare sectors read vs requested
    jne disk_error

    popa
    ret

disk_error:
    mov si, MSG_DISK_ERR
    call print_string_16
    jmp $

; ============================================================================
; Switch to Protected Mode
; ============================================================================

switch_to_pm:
    cli                     ; Disable interrupts

    lgdt [gdt_descriptor]   ; Load GDT

    ; Set PE (Protection Enable) bit in CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to 32-bit code (flushes pipeline)
    jmp CODE_SEG:init_pm

; ============================================================================
; Global Descriptor Table (GDT)
; ============================================================================

gdt_start:

gdt_null:                   ; Null descriptor (required)
    dd 0x0
    dd 0x0

gdt_code:                   ; Code segment descriptor
    dw 0xFFFF               ; Limit (bits 0-15)
    dw 0x0                  ; Base (bits 0-15)
    db 0x0                  ; Base (bits 16-23)
    db 10011010b            ; Access byte
    db 11001111b            ; Flags + Limit (bits 16-19)
    db 0x0                  ; Base (bits 24-31)

gdt_data:                   ; Data segment descriptor
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ============================================================================
; 32-bit Protected Mode
; ============================================================================

[bits 32]

init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Print message (directly to video memory)
    mov esi, MSG_PMODE
    mov edi, 0xB8000 + 160  ; Second line of video memory
    call print_string_32

    ; Jump to kernel
    call KERNEL_OFFSET

    jmp $                   ; Hang if kernel returns

; Print string in 32-bit mode (SI = string, DI = video memory)
print_string_32:
    pusha
    mov ah, 0x0F            ; White on black attribute
.loop:
    lodsb
    cmp al, 0
    je .done
    mov [edi], ax
    add edi, 2
    jmp .loop
.done:
    popa
    ret

; ============================================================================
; Data
; ============================================================================

BOOT_DRIVE:     db 0
MSG_BOOT:       db "MyOS Bootloader started...", 13, 10, 0
MSG_LOAD:       db "Loading kernel...", 13, 10, 0
MSG_DISK_ERR:   db "Disk read error!", 0
MSG_PMODE:      db "Entered 32-bit Protected Mode", 0

; ============================================================================
; Boot Sector Padding and Magic Number
; ============================================================================

times 510 - ($ - $$) db 0   ; Pad to 510 bytes
dw 0xAA55                   ; Boot sector magic number
