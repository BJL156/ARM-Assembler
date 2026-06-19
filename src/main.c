#include "elf64.h"
#include "encoder.h"
#include "lexer.h"
#include "parser.h"
#include "symtab.h"

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

  uint64_t load_addr = 0x400000;
  uint64_t file_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
  uint32_t pc = (uint32_t)(load_addr + file_offset);
  
  SymTab symtab;
  symtab_init(&symtab);

  for (int i = 0; i < program.count; i++) {
    Stmt *stmt = &program.stmts[i];
    if (stmt->type == STMT_LABEL) {
      symtab_define(&symtab, stmt->label, pc);
    } else if (stmt->type == STMT_INSTR) {
      pc += 4;
    }
  }

  int instr_count = 0;
  for (int i = 0; i < program.count; i++) {
    if (program.stmts[i].type == STMT_INSTR) {
      instr_count++;
    }
  }

  uint32_t *text = malloc(instr_count * sizeof(uint32_t));
  pc = (uint32_t)(load_addr + file_offset);
  int j = 0;

  for (int i = 0; i < program.count; i++) {
    Stmt *stmt = &program.stmts[i];
    if (stmt->type == STMT_INSTR) {
      text[j++] = encode_instr(stmt, pc, &symtab);
      pc += 4;
    }
  }

  FILE *output = fopen(argv[2], "wb");
  if (!output) {
    fprintf(stderr, "Error: Failed to open \"%s\".\n", argv[2]);
    return 1;
  }
  write_elf64(output, text, instr_count * sizeof(uint32_t));

  free(text);
  symtab_free(&symtab);
  program_free(&program);

  return 0;
}
