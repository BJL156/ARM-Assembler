#include "encoder.h"
#include "parser.h"

#include <stdio.h>
#include <strings.h>

uint32_t encode_svc(Stmt *stmt) {
  uint16_t imm = (uint16_t)stmt->instr.operands[0].imm;
  return 0xD4000001 | ((uint32_t)imm << 5);
}

uint32_t encode_mov(Stmt *stmt) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: mov takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *dst = &stmt->instr.operands[0];
  Operand *src = &stmt->instr.operands[1];

  if (dst->type != OP_REG) {
    fprintf(stderr, "Error: mov dst must be a register at line: %d", stmt->line);
    return 0;
  }

  if (src->type != OP_IMM) {
    fprintf(stderr, "Error: mov src must be an immediate at line: %d", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)dst->reg & 0x1F;
  uint32_t imm = (uint32_t)src->imm & 0xFFFF;
  return 0xD2800000 | (imm << 5) | rd;
}

uint32_t encode_add(Stmt *stmt) {
  if (stmt->instr.operand_count != 3) {
    fprintf(stderr, "Error: add takes 3 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *dst = &stmt->instr.operands[0];
  Operand *src = &stmt->instr.operands[1];
  Operand *op2 = &stmt->instr.operands[2];

  if (dst->type != OP_REG || src->type != OP_REG) {
    fprintf(stderr, "Error: add first two operands must be registers at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)dst->reg & 0x1F;
  uint32_t rn = (uint32_t)src->reg & 0x1F;

  if (op2->type == OP_IMM) {
    uint32_t imm12 = (uint32_t)op2->imm & 0xFFF;
    return 0x91000000 | (imm12 << 10) | (rn << 5) | rd;
  } else if (op2->type == OP_REG) {
    uint32_t rm = (uint32_t)op2->reg & 0x1F;
    return 0x8B000000 | (rm << 16) | (rn << 5) | rd;
  }

  fprintf(stderr, "Error: add third operand must be a register or immediate at line: %d.\n", stmt->line);
  return 0;
}

uint32_t encode_sub(Stmt *stmt) {
  if (stmt->instr.operand_count != 3) {
    fprintf(stderr, "Error: sub takes 3 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *dst = &stmt->instr.operands[0];
  Operand *src = &stmt->instr.operands[1];
  Operand *op2 = &stmt->instr.operands[2];

  if (dst->type != OP_REG || src->type != OP_REG) {
    fprintf(stderr, "Error: sub first two operands must be registers at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)dst->reg & 0x1F;
  uint32_t rn = (uint32_t)src->reg & 0x1F;

   if (op2->type == OP_IMM) {
    uint32_t imm12 = (uint32_t)op2->imm & 0xFFF;
    return 0xD1000000 | (imm12 << 10) | (rn << 5) | rd;
  } else if (op2->type == OP_REG) {
    uint32_t rm = (uint32_t)op2->reg & 0x1F;
    return 0xCB000000 | (rm << 16) | (rn << 5) | rd;
  }

  fprintf(stderr, "Error: add third operand must be a register or immediate at line: %d.\n", stmt->line);
  return 0;
}

uint32_t encode_ret(Stmt *stmt) {
  uint32_t rn = 30;

  if (stmt->instr.operand_count == 1) {
    if (stmt->instr.operands[0].type != OP_REG) {
      fprintf(stderr, "Error: ret operand must be a register at line: %d.\n", stmt->line);
      return 0;
    }

    rn = (uint32_t)stmt->instr.operands[0].reg & 0x1F;
  }

  return 0xD65F0000 | (rn << 5);
}

uint32_t encode_branch(Stmt *stmt, uint32_t base, uint32_t pc, SymTab *symtab) {
  if (stmt->instr.operand_count != 1) {
    fprintf(stderr, "Error: branch takes 1 operand at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *op = &stmt->instr.operands[0];

  int32_t offset = 0;
  if (op->type == OP_IMM) {
    offset = (int32_t)(op->imm - pc) / 4;
  } else if (op->type == OP_LABEL) {
    uint32_t target = 0;
    if (!symtab_lookup(symtab, op->label, &target)) {
      fprintf(stderr, "Error: undefined label \"%s\" at line: %d", op->label, stmt->line);
      return 0;
    }

    offset = (int32_t)(target - pc) / 4;
  } else {
    fprintf(stderr, "Error: branch operand must be an immediate or label at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t imm26 = (uint32_t)offset & 0x3FFFFFF;
  return base | imm26;
}

uint32_t encode_b(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_branch(stmt, 0x14000000, pc, symtab);
}

uint32_t encode_bl(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_branch(stmt, 0x94000000, pc, symtab);
}

uint32_t encode_instr(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  char *m = stmt->instr.mnemonic;
  if (strcasecmp(m, "mov") == 0) return encode_mov(stmt);
  if (strcasecmp(m, "add") == 0) return encode_add(stmt);
  if (strcasecmp(m, "sub") == 0) return encode_sub(stmt);
  if (strcasecmp(m, "svc") == 0) return encode_svc(stmt);
  if (strcasecmp(m, "b") == 0) return encode_b(stmt, pc, symtab);
  if (strcasecmp(m, "bl") == 0) return encode_bl(stmt, pc, symtab);
  if (strcasecmp(m, "ret") == 0) return encode_ret(stmt);

  fprintf(stderr, "Error: unknown mnemonic \"%s\" at line: %d.\n", m, stmt->line);
  return 0;
}
