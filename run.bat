@echo off
REM Run MyOS in QEMU
echo Starting MyOS in QEMU...
"C:\Program Files\qemu\qemu-system-i386.exe" -drive format=raw,file=build\os-image.bin,if=floppy -drive format=raw,file=build\os-image.bin,if=ide
