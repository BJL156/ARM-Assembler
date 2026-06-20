#include "elf64.h"
#include "encoder.h"
#include "layout.h"

#include <elf.h>
#include <string.h>

typedef enum {
  SECTION_TEXT,
  SECTION_DATA
} Section;

void layout_init(Layout *layout, Program *program) {
  uint64_t ehdr_phdr_size = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) * 2;

  size_t instr_count = 0;
  size_t data_size = 0;

  for (int i = 0; i < program->count; i++) {
    Stmt *stmt = &program->stmts[i];
    if (stmt->type == STMT_INSTR) {
      instr_count++;
    } else if (stmt->type == STMT_DIRECTIVE && strcasecmp(stmt->directive.name, "asciz") == 0) {
      data_size += strlen(stmt->directive.arg) + 1;
    }
  }

  layout->text_size = instr_count * sizeof(uint32_t);
  layout->data_size = data_size;

  layout->text_vaddr    = LOAD_ADDR + ehdr_phdr_size;
  layout->text_seg_size = ehdr_phdr_size + layout->text_size;

  uint64_t text_end   = layout->text_seg_size;
  layout->data_offset = (text_end + PAGE_SIZE - 1) & ~((uint64_t)PAGE_SIZE - 1);
  layout->data_vaddr  = LOAD_ADDR + layout->data_offset;
}

void layout_create_symtab(Layout *layout, Program *program, SymTab *symtab) {
  Section section = SECTION_TEXT;
  uint32_t pc = (uint32_t)layout->text_vaddr;
  uint64_t data_cursor = layout->data_vaddr;

  for (int i = 0; i < program->count; i++) {
    Stmt *stmt = &program->stmts[i];

    if (stmt->type == STMT_DIRECTIVE) {
      if (strcasecmp(stmt->directive.name, "text") == 0) {
        section = SECTION_TEXT;
      } else if (strcasecmp(stmt->directive.name, "data") == 0) {
        section = SECTION_DATA;
      } else if (strcasecmp(stmt->directive.name, "asciz") == 0) {
        data_cursor += strlen(stmt->directive.arg) + 1;
      }
      continue;
    }

    if (stmt->type == STMT_LABEL) {
      uint32_t addr = (section == SECTION_TEXT) ? pc : (uint32_t)data_cursor;
      symtab_define(symtab, stmt->label, addr);
      continue;
    }

    if (stmt->type == STMT_INSTR) {
      pc += 4;
    }
  }
}

void layout_encode_program(Layout *layout, Program *program, SymTab *symtab, uint32_t *text, uint8_t *data) {
  uint32_t pc = (uint32_t)layout->text_vaddr;
  size_t data_pos = 0;
  int j = 0;

  for (int i = 0; i < program->count; i++) {
    Stmt *stmt = &program->stmts[i];

    if (stmt->type == STMT_DIRECTIVE) {
      if (strcasecmp(stmt->directive.name, "asciz") == 0) {
        size_t len = strlen(stmt->directive.arg) + 1;
        memcpy(data + data_pos, stmt->directive.arg, len);
        data_pos += len;
      }
      continue;
    }

    if (stmt->type == STMT_INSTR) {
      text[j++] = encode_instr(stmt, pc, symtab);
      pc += 4;
    }
  }
}
