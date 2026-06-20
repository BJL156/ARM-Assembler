#ifndef LAYOUT_H
#define LAYOUT_H

#include "parser.h"
#include "symtab.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t text_vaddr;
  uint64_t text_seg_size;
  uint64_t data_offset;
  uint64_t data_vaddr;
  size_t text_size;
  size_t data_size;
} Layout;

void layout_init(Layout *layout, Program *program);
void layout_create_symtab(Layout *layout, Program *program, SymTab *symtab);
void layout_encode_program(Layout *layout, Program *program, SymTab *symtab, uint32_t *text, uint8_t *data);

#endif
