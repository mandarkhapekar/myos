@echo off
REM ============================================================================
REM MyOS Build Script for Windows
REM ============================================================================
REM Prerequisites:
REM   - NASM: https://www.nasm.us/ (add to PATH)
REM   - i686-elf cross compiler OR use WSL
REM   - QEMU: https://www.qemu.org/download/#windows (add to PATH)
REM ============================================================================

echo.
echo ============================================
echo           MyOS Build Script
echo ============================================
echo.

REM Navigate to project root
pushd %~dp0\..

REM Create build directory
if not exist build mkdir build

REM Check for NASM
where nasm >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] NASM not found! Please install NASM and add to PATH.
    echo Download from: https://www.nasm.us/
    exit /b 1
)

echo [1/5] Assembling bootloader...
nasm -f bin boot\boot.asm -o build\boot.bin
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to assemble bootloader!
    exit /b 1
)
echo       Done!

echo [2/5] Assembling kernel entry...
nasm -f elf32 kernel\kernel_entry.asm -o build\kernel_entry.o
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to assemble kernel entry!
    exit /b 1
)
echo       Done!

REM Try different compiler options
echo [3/5] Compiling kernel C files...

REM Check for i686-elf-gcc (cross compiler)
where i686-elf-gcc >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo       Using i686-elf-gcc cross compiler
    set CC=i686-elf-gcc
    set LD=i686-elf-ld
    goto :compile
)

REM Check for i386-elf-gcc
where i386-elf-gcc >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo       Using i386-elf-gcc cross compiler
    set CC=i386-elf-gcc
    set LD=i386-elf-ld
    goto :compile
)

REM If no cross compiler, show instructions
echo.
echo [ERROR] No i686-elf cross compiler found!
echo.
echo You need a cross compiler for OS development. Options:
echo.
echo Option 1: Use WSL (Windows Subsystem for Linux)
echo   1. Install WSL: wsl --install
echo   2. In WSL, run: sudo apt install gcc-i686-linux-gnu nasm
echo   3. Use build_wsl.sh instead
echo.
echo Option 2: Download pre-built cross compiler
echo   https://github.com/lordmilko/i686-elf-tools/releases
echo   Extract and add bin folder to PATH
echo.
echo Option 3: Build cross compiler (advanced)
echo   https://wiki.osdev.org/GCC_Cross-Compiler
echo.
exit /b 1

:compile
%CC% -ffreestanding -m32 -c kernel\kernel.c -o build\kernel.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\screen.c -o build\screen.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\keyboard.c -o build\keyboard.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\filesystem.c -o build\filesystem.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\shell.c -o build\shell.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\memory.c -o build\memory.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\math.c -o build\math.o -fno-pie -fno-stack-protector
%CC% -ffreestanding -m32 -c kernel\ata.c -o build\ata.o -fno-pie -fno-stack-protector

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to compile kernel!
    exit /b 1
)
echo       Done!

echo [4/5] Linking kernel...
%LD% -o build\kernel.bin -T kernel\linker.ld build\kernel_entry.o build\kernel.o build\screen.o build\keyboard.o build\filesystem.o build\shell.o build\memory.o build\math.o build\ata.o --oformat binary -m elf_i386
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to link kernel!
    exit /b 1
)
echo       Done!

echo [5/5] Creating OS image...
copy /b build\boot.bin+build\kernel.bin build\os-image.bin >nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to create OS image!
    exit /b 1
)
echo       Done!

echo.
echo ============================================
echo           Build Successful!
echo ============================================
echo.
echo Output: build\os-image.bin
echo.
echo To run in QEMU:
echo   qemu-system-i386 -hda build\os-image.bin
echo.
echo To run in VirtualBox:
echo   1. Create new VM (Type: Other, Version: Other/Unknown)
echo   2. Add floppy controller
echo   3. Attach build\os-image.bin as floppy
echo   4. Boot from floppy
echo.
