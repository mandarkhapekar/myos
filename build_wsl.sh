#!/bin/bash
# ============================================================================
# MyOS Build Script for WSL/Linux
# ============================================================================

set -e

echo ""
echo "============================================"
echo "          MyOS Build Script (WSL)"
echo "============================================"
echo ""

# Create build directory
mkdir -p build

# Determine compiler
if command -v i686-elf-gcc &> /dev/null; then
    CC=i686-elf-gcc
    LD=i686-elf-ld
elif command -v i686-linux-gnu-gcc &> /dev/null; then
    CC=i686-linux-gnu-gcc
    LD=i686-linux-gnu-ld
else
    echo "[ERROR] No cross compiler found!"
    echo "Install with: sudo apt install gcc-i686-linux-gnu"
    exit 1
fi

echo "Using compiler: $CC"

echo "[1/5] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/5] Assembling kernel entry..."
nasm -f elf32 kernel/kernel_entry.asm -o build/kernel_entry.o

echo "[3/5] Compiling kernel..."
$CC -ffreestanding -m32 -c kernel/kernel.c -o build/kernel.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/screen.c -o build/screen.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/keyboard.c -o build/keyboard.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/filesystem.c -o build/filesystem.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/shell.c -o build/shell.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/memory.c -o build/memory.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/math.c -o build/math.o -fno-pie -fno-stack-protector
$CC -ffreestanding -m32 -c kernel/ata.c -o build/ata.o -fno-pie -fno-stack-protector

echo "[4/5] Linking kernel..."
$LD -o build/kernel.bin -T kernel/linker.ld \
    build/kernel_entry.o build/kernel.o build/screen.o \
    build/keyboard.o build/filesystem.o build/shell.o build/memory.o build/math.o build/ata.o \
    --oformat binary -m elf_i386

echo "[5/5] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os-image.bin

# Pad to 1.44MB floppy size for QEMU compatibility
truncate -s 1474560 build/os-image.bin

echo ""
echo "============================================"
echo "          Build Successful!"
echo "============================================"
echo ""
echo "Output: build/os-image.bin"
echo ""
echo "To run: qemu-system-i386 -fda build/os-image.bin"
echo ""
