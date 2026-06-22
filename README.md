# ARM Assembler
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)
[![Instructions](https://img.shields.io/badge/instructions-17-success.svg)](#Supported-Instructions)

A small AArch64 assembler written entirely from scratch in C. It translates AArch64 source code into a Linux ELF64 binary that can be run on real hardware or through emulators such as QEMU.

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
    - [x] Directives.
    - [x] Immediates.
    - [x] Registers.
    - [x] Mnemonics.
    - [x] Strings.
- Parser.
  - [x] Converts Tokens into statements (`Stmt`).
  - [x] Stores a dynamic array of statements (`Program`).
- Encoder.
  - [x] Converts all supported mnemonics into machine code.
- ELF Builder.
  - [x] Combines ELF header, Program header, and all machine code into a binary that can be ran within an AArch64 Linux enviroment or using an emulator.

## Supported Instructions
| Category | Instructions |
|:----------|:------------|
| Data Movement | `mov`, `ldr`, `str`, `ldrb`, `strb`, `adr` |
| Arithmetic | `add`, `sub` |
| Comparison | `cmp` |
| Branching | `b`, `bl`, `b.eq`, `b.ne`, `ret`, `cbz`, `cbnz` |
| System / Hints | `svc`, `nop` |
