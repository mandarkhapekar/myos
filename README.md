# 🚀 MyOS

> **A tiny operating system built from scratch in a weekend.**  
> *No libraries. No frameworks. Just raw metal.*

```
  __  __        ___  ____  
 |  \/  |_   _ / _ \/ ___| 
 | |\/| | | | | | | \___ \ 
 | |  | | |_| | |_| |___) |
 |_|  |_|\__, |\___/|____/ 
         |___/             
```

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Made with Assembly](https://img.shields.io/badge/Made%20with-Assembly-blueviolet)](https://en.wikipedia.org/wiki/Assembly_language)
[![Made with C](https://img.shields.io/badge/Made%20with-C-blue)](https://en.wikipedia.org/wiki/C_(programming_language))

---

## 🚨 Coming Soon: MyOS v2.0 with Native AI!

> **We're building the world's first hobby OS with built-in LLM inference.**

```
myos> ai tell me a story about a robot

🤖 Once upon a time, there was a little robot named Chip...
```

No CUDA. No PyTorch. No Python. Just bare-metal C running a neural network directly on your CPU.

**[📋 Read the full v2.0 Roadmap →](ROADMAP.md)**

---

## ✨ What is this?

MyOS is a **primitive operating system** built completely from scratch. No Linux kernel. No GRUB. Just a bootloader written in x86 assembly that switches to protected mode and loads a kernel written in C.

**It boots. It runs. It has a shell. It can create files.**

Perfect for learning how computers *actually* work at the lowest level.

---

## 🎮 Features

| Feature | Status |
|---------|--------|
| 🔧 Custom bootloader | ✅ |
| 🖥️ VGA text mode display | ✅ |
| ⌨️ PS/2 keyboard input | ✅ |
| 📁 In-memory file system | ✅ |
| 💻 Interactive shell | ✅ |
| 🎨 Color output | ✅ |

---

## 🖥️ Shell Commands

```
myos> help

  help              - Show this help
  clear             - Clear the screen
  about             - About MyOS
  list              - List all files
  create <file>     - Create a new file
  read <file>       - Read file contents
  write <file> <txt>- Write text to file
  delete <file>     - Delete a file
```

---

## 🛠️ Building

### Prerequisites

- **NASM** - `winget install nasm` or `apt install nasm`
- **Cross-compiler** - `apt install gcc-i686-linux-gnu` (in WSL/Linux)
- **QEMU** - `winget install qemu` or `apt install qemu-system-x86`

### Build & Run

**On macOS:**
```bash
./scripts/build_mac.sh
./scripts/run_mac.sh
```

**On WSL/Linux:**
```bash
./scripts/build_wsl.sh
qemu-system-i386 -fda build/os-image.bin
```

**On Windows (with WSL for building):**
```cmd
wsl ./scripts/build_wsl.sh
.\scripts\run.bat
```

---

## 📚 How It Works

```
┌─────────────────────────────────────────────────────────────┐
│  1. BIOS loads bootloader (512 bytes)                       │
│  2. Bootloader switches CPU to 32-bit protected mode        │
│  3. Bootloader loads kernel from disk                       │
│  4. Kernel initializes VGA, keyboard, filesystem            │
│  5. Shell runs and waits for your commands!                 │
└─────────────────────────────────────────────────────────────┘
```

### Project Structure

```
myos/
├── boot/
│   └── boot.asm          # 💾 Bootloader (fits in 512 bytes!)
├── kernel/
│   ├── kernel_entry.asm  # 🚪 Entry point from bootloader
│   ├── kernel.c          # 🧠 Main kernel
│   ├── screen.c          # 🖥️ VGA driver
│   ├── keyboard.c        # ⌨️ Keyboard driver
│   ├── filesystem.c      # 📁 Simple file system
│   └── shell.c           # 💻 Command shell
├── scripts/
│   ├── build_mac.sh      # 🔨 Build script (macOS)
│   ├── build_wsl.sh      # 🔨 Build script (WSL/Linux)
│   ├── run_mac.sh        # ▶️ Run script (macOS)
│   └── run.bat           # ▶️ Run script (Windows)
└── build/                # 📦 Build artifacts
```

---

## 🎓 Learn OS Development

This project is perfect for learning:

- **x86 Assembly** - Real mode, protected mode, GDT
- **Hardware I/O** - Port-based communication with keyboard/display
- **Memory-mapped I/O** - Writing directly to VGA memory
- **Bare metal C** - No stdlib, no heap, no safety net
- **Build systems** - Cross-compilation, linker scripts

### Resources

- 📖 [OSDev Wiki](https://wiki.osdev.org/) - The bible of OS development
- 📄 [Writing an OS from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) - Excellent PDF guide
- 🎥 [Nanobyte OS Series](https://www.youtube.com/playlist?list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) - Video tutorials

---

## 🚧 Future Ideas

- [ ] Interrupt-driven keyboard (IDT)
- [ ] Memory allocator (malloc/free)
- [ ] Simple text editor
- [ ] Disk persistence (FAT12)
- [ ] User programs
- [ ] Multitasking

---

## 📜 License

MIT License - Do whatever you want with it! See [LICENSE](LICENSE).

---

<p align="center">
  <b>Built with ❤️ and way too much coffee</b><br>
  <i>Because sometimes you just need to know how it all works.</i>
</p>
