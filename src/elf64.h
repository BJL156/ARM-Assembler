#ifndef ELF64_H
#define ELF64_H

#include <stdint.h>
#include <stdio.h>

#define LOAD_ADDR 0x400000
#define PAGE_SIZE 0x10000

void write_elf64(FILE *file, uint32_t *text, size_t text_size, uint8_t *data, size_t data_size);

#endif
