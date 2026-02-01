@echo off
REM Run MyOS in QEMU
REM Navigate to project root
pushd %~dp0\..

echo Starting MyOS in QEMU...
"C:\Program Files\qemu\qemu-system-i386.exe" -drive file=build\os-image.bin,format=raw,index=0,media=disk
