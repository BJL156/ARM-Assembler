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
    fprintf(stderr, "Error: mov dst must be a register at line: %d.\n", stmt->line);
    return 0;
  }

  if (src->type == OP_REG) {
    uint32_t rd = (uint32_t)dst->reg & 0x1F;
    uint32_t rm = (uint32_t)src->reg & 0x1F;
    return 0xAA0003E0 | (rm << 16) | rd;
  }

  if (src->type != OP_IMM) {
    fprintf(stderr, "Error: mov src must be a register and or immediate at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)dst->reg & 0x1F;

  if (src->imm < 0) {
    uint32_t imm16 = (uint32_t)(~src->imm) & 0xFFFF;
    return 0x92800000 | (imm16 << 5) | rd;
  } else {
    uint32_t imm16 = (uint32_t)src->imm & 0xFFFF;
    return 0xD2800000 | (imm16 << 5) | rd;
  }
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
      fprintf(stderr, "Error: undefined label \"%s\" at line: %d.\n", op->label, stmt->line);
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

uint32_t encode_cbx(Stmt *stmt, uint32_t base, uint32_t pc, SymTab *symtab) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: cbz/cbnz takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *reg = &stmt->instr.operands[0];
  Operand *target = &stmt->instr.operands[1];

  if (reg->type != OP_REG) {
    fprintf(stderr, "Error: cbz/cbnz first operand must be a register at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rt = (uint32_t)reg->reg & 0x1F;

  int32_t offset = 0;
  if (target->type == OP_LABEL) {
    uint32_t addr = 0;
    if (!symtab_lookup(symtab, target->label, &addr)) {
      fprintf(stderr, "Error: undefined label \"%s\" at line: %d.\n", target->label, stmt->line);
      return 0;
    }
    offset = (int32_t)(addr - pc) / 4;
  } else if (target->type == OP_IMM) {
    offset = (int32_t)(target->imm - pc) / 4;
  } else {
    fprintf(stderr, "Error: cbz/cbnz second operand must be a label at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t imm19 = (uint32_t)offset & 0x7FFFF;
  return base | (imm19 << 5) | rt;
}

uint32_t encode_cbz(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_cbx(stmt, 0xB4000000, pc, symtab);
}

uint32_t encode_cbnz(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_cbx(stmt, 0xB5000000, pc, symtab);
}

uint32_t encode_cmp(Stmt *stmt) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: cmp takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *lhs = &stmt->instr.operands[0];
  Operand *rhs = &stmt->instr.operands[1];

  if (lhs->type != OP_REG) {
    fprintf(stderr, "Error: cmp first operand must be a register at lnie: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rn = (uint32_t)lhs->reg & 0x1F;
  uint32_t rd = 31;

  if (rhs->type == OP_IMM) {
    uint32_t imm12 = (uint32_t)rhs->imm & 0xFFF;
    return 0xF1000000 | (imm12 << 10) | (rn << 5) | rd;
  } else if (rhs->type == OP_REG) {
    uint32_t rm = (uint32_t)rhs->reg & 0x1F;
    return 0xEB000000 | (rm << 16) | (rn << 5) | rd;
  }

  fprintf(stderr, "Error: cmp second operand must be a register and immediate at line: %d.\n", stmt->line);
  return 0;
}

uint32_t encode_bcond(Stmt *stmt, uint32_t cond, uint32_t pc, SymTab *symtab) {
  if (stmt->instr.operand_count != 1) {
    fprintf(stderr, "Error: b.cond takes 1 operand at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *target = &stmt->instr.operands[0];

  int32_t offset = 0;
  if (target->type == OP_LABEL) {
    uint32_t addr = 0;
    if (!symtab_lookup(symtab, target->label, &addr)) {
      fprintf(stderr, "Error: undefined label \"%s\" at line: %d.\n", target->label, stmt->line);
      return 0;
    }
    offset = (int32_t)(addr - pc) / 4;
  } else if (target->type == OP_IMM) {
    offset = (int32_t)(target->imm - pc) / 4;
  } else {
    fprintf(stderr, "Error: b.cond operand must be a label or immediate at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t imm19 = (uint32_t)offset & 0x7FFFF;
  return 0x54000000 | (imm19 << 5) | cond;
}

uint32_t encode_beq(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_bcond(stmt, 0x0, pc, symtab);
}

uint32_t encode_bne(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  return encode_bcond(stmt, 0x1, pc, symtab);
}

uint32_t encode_ldr_str(Stmt *stmt, uint32_t base) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: ldr/str takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *rt = &stmt->instr.operands[0];
  Operand *mem = &stmt->instr.operands[1];

  if (rt->type != OP_REG || mem->type != OP_MEM) {
    fprintf(stderr, "Error: ldr/str expects register, [register, immediate] at line: %d.\n", stmt->line);
    return 0;
  }

  if (mem->mem.offset % 8 != 0) {
    fprintf(stderr, "Error: ldr/str offset must be divisible by 8 at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)rt->reg & 0x1F;
  uint32_t rn = (uint32_t)mem->mem.base_reg & 0x1F;
  uint32_t imm12 = (uint32_t)(mem->mem.offset / 8) & 0xFFF;

  return base | (imm12 << 10) | (rn << 5) | rd;
}

uint32_t encode_ldr(Stmt *stmt) {
  return encode_ldr_str(stmt, 0xF9400000);
}

uint32_t encode_str(Stmt *stmt) {
  return encode_ldr_str(stmt, 0xF9000000);
}

uint32_t encode_nop() {
  return 0xD503201F;
}

uint32_t encode_adr(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: adr takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *dst = &stmt->instr.operands[0];
  Operand *target = &stmt->instr.operands[1];

  if (dst->type != OP_REG || target->type != OP_LABEL) {
    fprintf(stderr, "Error: adr expects register, label at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t addr = 0;
  if (!symtab_lookup(symtab, target->label, &addr)) {
    fprintf(stderr, "Error: undefined label \"%s\" at line: %d.\n", target->label, stmt->line);
    return 0;
  }

  int32_t offset = (int32_t)(addr - pc);
  uint32_t rd = (uint32_t)dst->reg & 0x1F;
  uint32_t immlo = (uint32_t)offset & 0x3;
  uint32_t immhi = ((uint32_t)offset >> 2) & 0x7FFFF;

  return 0x10000000 | (immlo << 29) | (immhi << 5) | rd;
}

uint32_t encode_ldrb_strb(Stmt *stmt, uint32_t base) {
  if (stmt->instr.operand_count != 2) {
    fprintf(stderr, "Error: ldrb/strb takes 2 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *rt = &stmt->instr.operands[0];
  Operand *mem = &stmt->instr.operands[1];

  if (rt->type != OP_REG || mem->type != OP_MEM) {
    fprintf(stderr, "Error: ldrb/strb expects register, [register, immediate] at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)rt->reg & 0x1F;
  uint32_t rn = (uint32_t)mem->mem.base_reg & 0x1F;
  uint32_t imm12 = (uint32_t)mem->mem.offset & 0xFFF;

  return base | (imm12 << 10) | (rn << 5) | rd;
}

uint32_t encode_ldrb(Stmt *stmt) {
  return encode_ldrb_strb(stmt, 0x39400000);
}

uint32_t encode_strb(Stmt *stmt) {
  return encode_ldrb_strb(stmt, 0x39000000);
}

uint32_t encode_mul(Stmt *stmt) {
  if (stmt->instr.operand_count != 3) {
    fprintf(stderr, "Error: mul takes 3 operands at line: %d.\n", stmt->line);
    return 0;
  }

  Operand *dst = &stmt->instr.operands[0];
  Operand *src = &stmt->instr.operands[1];
  Operand *op2 = &stmt->instr.operands[2];

  if (dst->type != OP_REG || src->type != OP_REG || op2->type != OP_REG) {
    fprintf(stderr, "Error: mul takes register, register, register at line: %d.\n", stmt->line);
    return 0;
  }

  uint32_t rd = (uint32_t)dst->reg & 0x1F;
  uint32_t rn = (uint32_t)src->reg & 0x1F;
  uint32_t rm = (uint32_t)op2->reg & 0x1F;

  return 0x9B007C00 | (rm << 16) | (rn << 5) | rd;
}

uint32_t encode_instr(Stmt *stmt, uint32_t pc, SymTab *symtab) {
  char *m = stmt->instr.mnemonic;
  if (strcasecmp(m, "mov") == 0)  return encode_mov(stmt);
  if (strcasecmp(m, "add") == 0)  return encode_add(stmt);
  if (strcasecmp(m, "sub") == 0)  return encode_sub(stmt);
  if (strcasecmp(m, "svc") == 0)  return encode_svc(stmt);
  if (strcasecmp(m, "b") == 0)    return encode_b(stmt, pc, symtab);
  if (strcasecmp(m, "bl") == 0)   return encode_bl(stmt, pc, symtab);
  if (strcasecmp(m, "ret") == 0)  return encode_ret(stmt);
  if (strcasecmp(m, "cbz") == 0)  return encode_cbz(stmt, pc, symtab);
  if (strcasecmp(m, "cbnz") == 0) return encode_cbnz(stmt, pc, symtab);
  if (strcasecmp(m, "cmp") == 0)  return encode_cmp(stmt);
  if (strcasecmp(m, "b.eq") == 0) return encode_beq(stmt, pc, symtab);
  if (strcasecmp(m, "b.ne") == 0) return encode_bne(stmt, pc, symtab);
  if (strcasecmp(m, "ldr") == 0)  return encode_ldr(stmt);
  if (strcasecmp(m, "str") == 0)  return encode_str(stmt);
  if (strcasecmp(m, "nop") == 0)  return encode_nop();
  if (strcasecmp(m, "adr") == 0)  return encode_adr(stmt, pc, symtab);
  if (strcasecmp(m, "ldrb") == 0) return encode_ldrb(stmt);
  if (strcasecmp(m, "strb") == 0) return encode_strb(stmt);
  if (strcasecmp(m, "mul") == 0)  return encode_mul(stmt);

  fprintf(stderr, "Error: unknown mnemonic \"%s\" at line: %d.\n", m, stmt->line);
  return 0;
}
