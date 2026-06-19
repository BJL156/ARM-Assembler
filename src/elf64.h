#ifndef ELF64_H
#define ELF64_H

#include <stdint.h>
#include <stdio.h>

void write_elf64(FILE *file, uint32_t *text, size_t text_size);

#endif
