# ARM Assembler
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)
[![Instructions](https://img.shields.io/badge/instructions-13-success.svg)](#Supported-Instructions)

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
### Input (`asm/return.s`)
```s
.global _start
_start:
  mov x8, #93
  mov x0, #67
  svc #0
```
### Output
```bash
$ ./build/assembler asm/return.s build/return
$ ./build/return (or qemu-aarch64 ./build/return)
$ echo $?
67
```

## Features
- Lexer.
  - [x] Converts source into tokens.
  - [x] Handles whitespaces and comments.
  - [x] Scans:
    - [x] End of file (`EOF`).
    - [x] New lines.
    - [x] Directives.
    - [x] Immediates.
    - [x] Registers.
    - [x] Mnemonics.
- Parser.
  - [x] Converts Tokens into statements (`Stmt`).
  - [x] Creates a dynamic array of statements (`Program`).
- Encoder.
  - [x] Converts all mnemonics into machine code.
- ELF Builder.
  - [x] Combines ELF header, Program header, and all machine code into a binary.

## Support Instructions
| Category | Instructions |
|:---------:|:------------|
| Data Movement | `mov`, `ldr`, `str` |
| Arithmetic | `add`, `sub` |
| Comparison | `cmp` |
| Branching | `b`, `bl`, `ret`, `cbz`, `cbnz`, `b.eq`, `b.ne` |
| System | `svc` |
