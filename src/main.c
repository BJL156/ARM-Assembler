#include "elf64.h"
#include "layout.h"
#include "lexer.h"
#include "parser.h"
#include "symtab.h"

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

static char *read_file(const char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (!file) {
    fprintf(stderr, "Error: Failed to open \"%s\".\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, file);
  buffer[size] = '\0';

  fclose(file);
  return buffer;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input.s> <output>\n", argv[0]);
    return 1;
  }

  char *src = read_file(argv[1]);
  if (!src) {
    return 1;
  }

  Lexer lexer;
  lexer_init(&lexer, src);

  Program program;
  parse(&lexer, &program);
  free(src);

  SymTab symtab;
  symtab_init(&symtab); 

  Layout layout;
  layout_init(&layout, &program);
  layout_create_symtab(&layout, &program, &symtab);
  layout_init_entry(&layout, &program, &symtab);

  uint32_t *text = malloc(layout.text_size);
  uint8_t *data = malloc(layout.data_size);
  layout_encode_program(&layout, &program, &symtab, text, data);

  FILE *output = fopen(argv[2], "wb");
  if (!output) {
    fprintf(stderr, "Error: Failed to open \"%s\".\n", argv[2]);
    return 1;
  }
  write_elf64(output, text, layout.text_size, data, layout.data_size, layout.entry);
  fclose(output);

  free(text);
  free(data);
  symtab_free(&symtab);
  program_free(&program);

  return 0;
}
