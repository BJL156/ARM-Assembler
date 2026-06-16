# ARM Assembler
A simple AArch64 assembler written entirely in C from scratch. It converts ARM source code into an ELF64 binary that can be run using an emulator such as QEMU or native on real hardware.

## Build
### **Needs to be built on Linux. For Windows, use WSL.**
Clone the repository and change into its directory:
```
git clone https://github.com/BJL156/ARM-Assembler
cd ARM-Assembler
```
Then use CMake:
```
cmake -B build
cmake --build build
```
The final executable will be written to: `build/assembler`.

## Usage
```
./assembler <file.s> <output>
  <file.s>  assembly file to run.
  <output>  name of the final ELF binary.
```

## Example
### Input (`asm/return.s`)
```
.global _start
_start:
    mov x8, #93
    mov x0, #67
    svc #0
```
### Output
```
$ ./build/assembler asm/return.s build/hello
$ ./build/hello
$ echo $?
67
```

## Features
- Lexer.
  - [x] Converts source into tokens.
  - [x] Handles whitespaces and comments.
  - [X] Scans:
    - [x] End of file (`EOF`).
    - [x] New lines.
    - [x] Directives.
    - [x] Immediates.
    - [x] Registers.
    - [X] Mnemonics.
- Parser.
  - [x] Converts Tokens into statements (`Stmt`).
  - [x] Creates a dynamic array of statements (`Program`).
- Encoder.
  - [x] Converts all mnemonics into machine code.
- ELF Builder.
  - [x] Combines ELF header, Program header, and all machine code into a binary.
