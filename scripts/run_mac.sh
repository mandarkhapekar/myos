#!/bin/bash
# Run MyOS in QEMU (macOS)

# Navigate to project root
cd "$(dirname "$0")/.."

echo "Starting MyOS in QEMU..."
qemu-system-i386 -drive format=raw,file=build/os-image.bin,if=floppy,file.locking=off -drive format=raw,file=build/os-image.bin,if=ide,file.locking=off
