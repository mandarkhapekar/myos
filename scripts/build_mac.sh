#!/bin/bash
# ============================================================================
# MyOS Build Script for macOS
# ============================================================================

set -e

echo ""
echo "============================================"
echo "          MyOS Build Script (macOS)"
echo "============================================"
echo ""

# Navigate to project root
cd "$(dirname "$0")/.."

# Create build directory
mkdir -p build

# Determine compiler
if command -v i686-elf-gcc &> /dev/null; then
    CC=i686-elf-gcc
    LD=i686-elf-ld
else
    echo "[ERROR] No cross compiler found!"
    echo "Install with: brew install i686-elf-gcc"
    exit 1
fi

echo "Using compiler: $CC"

echo "[1/5] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/5] Assembling kernel entry..."
nasm -f elf32 kernel/kernel_entry.asm -o build/kernel_entry.o

echo "[3/5] Compiling kernel..."
# Added -std=gnu99 to avoid C23 'bool' keyword conflict
CFLAGS="-ffreestanding -m32 -std=gnu99 -fno-pie -fno-stack-protector"

$CC $CFLAGS -c kernel/kernel.c -o build/kernel.o
$CC $CFLAGS -c kernel/screen.c -o build/screen.o
$CC $CFLAGS -c kernel/keyboard.c -o build/keyboard.o
$CC $CFLAGS -c kernel/filesystem.c -o build/filesystem.o
$CC $CFLAGS -c kernel/shell.c -o build/shell.o
$CC $CFLAGS -c kernel/memory.c -o build/memory.o
$CC $CFLAGS -c kernel/math.c -o build/math.o
$CC $CFLAGS -c kernel/ata.c -o build/ata.o

echo "[4/5] Linking kernel..."
$LD -o build/kernel.bin -T kernel/linker.ld \
    build/kernel_entry.o build/kernel.o build/screen.o \
    build/keyboard.o build/filesystem.o build/shell.o build/memory.o build/math.o build/ata.o \
    --oformat binary -m elf_i386

echo "[5/5] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os-image.bin

# Pad to 1.44MB floppy size for QEMU compatibility
# Check for truncate or gtruncate
if command -v truncate &> /dev/null; then
    truncate -s 1474560 build/os-image.bin
elif command -v gtruncate &> /dev/null; then
    gtruncate -s 1474560 build/os-image.bin
else
    echo "[WARNING] truncate not found. Creating 1.44MB image using dd..."
    dd if=/dev/zero of=build/os-image.bin bs=1 count=1 seek=1474559 conv=notrunc > /dev/null 2>&1
fi

echo ""
echo "============================================"
echo "          Build Successful!"
echo "============================================"
echo ""
echo "Output: build/os-image.bin"
echo ""
echo "To run: qemu-system-i386 -fda build/os-image.bin"
echo ""
