# ARM Assembler
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)
[![Instructions](https://img.shields.io/badge/instructions-36-success.svg)](#Supported-Instructions)

A lightweight, zero-dependency AArch64 assembler written from scratch in C. It compiles AArch64 assembly source code directly into Linux ELF64 executables. It can be executed on bare-metal hardware or using emulators like QEMU.

This AArch64 assembler is the next step of my compiler AArch64 toolchain. it builds on my last project: [CPU Simulator](https://github.com/BJL156/CPU-Simulator). By targetting a real ISA.

## Build
> [!NOTE]
> **Needs to be built on Linux. For Windows, use WSL.**

Clone the repository and change into its directory:
```bash
git clone https://github.com/BJL156/ARM-Assembler
cd ARM-Assembler
```
Then use CMake:
```bash
cmake -B build
cmake --build build
```
The final executable will be written to: `build/assembler`.

## Usage
```bash
./assembler <file.s> <output>
  <file.s>  AArch64 assembly source file.
  <output>  Output ELF64 executable.
```

## Example
### Input (`asm/hello.s`)
```s
.global _start
.data
  msg: .asciz "Hello, world!\n"

.text
_start:
  adr x1, msg
  mov x2, #14
  mov x0, #1
  mov x8, #64
  svc #0

  mov x0, #0
  mov x8, #93
  svc #0
```
### Output
```bash
$ ./build/assembler ./asm/hello.s build/hello
$ ./build/hello (or qemu-aarch64 ./build/hello)
Hello, world!
$ echo $?
0
```

## Features
- Lexer.
  - [x] Converts AArch64 source into tokens.
  - [x] Handles whitespaces and comments.
  - [x] Scans:
    - [x] End of file (`EOF`).
    - [x] New lines.
    - [x] Directives (`text`, `data`, `asciz`, `byte`, `word`, `quad`, `space`).
    - [x] Immediates.
    - [x] 64 and 32 bit registers. (`x` and `w` respectively).
    - [x] Mnemonics.
    - [x] Strings.
- Parser.
  - [x] Converts Tokens into statements (`Stmt`).
  - [x] Stores a dynamic array of statements (`Program`).
- Encoder.
  - [x] Converts all supported mnemonics into machine code.
- ELF Builder.
  - [x] Combines the ELF header, program header, machine code, and data into an ELF64 that can be ran within an AArch64 Linux enviroment or using an emulator.

## Current Limitations
- Linux ELF64 output only.
- Small set of AArch64 instructions.
- No relocations.
- No linker support (converts straight to an ELF64).
- Single-file assembly programs only.

## Implemented AArch64 Instructions
| Category | Instructions |
|:----------|:------------|
| Data Movement | `mov`, `ldr`, `str`, `ldrb`, `strb`, `adr`, `ldp`, `stp` |
| Arithmetic | `add`, `sub`, `mul`, `udiv`, `sdiv`, `neg` |
| Shift | `lsl`, `lsr`, `asr` |
| Bitwise | `and`, `orr`, `eor`, `mvn` |
| Branching | `b`, `bl`, `b.eq`, `b.ne`, `b.lt`, `b.le`, `b.gt`, `b.ge`, `b.hi`, `b.lo`, `b.hs`, `b.ls`, `b.mi`, `b.pl`, `ret`, `cbz`, `cbnz` |
| Comparison | `cmp` |
| System / Hints | `svc`, `nop` |
