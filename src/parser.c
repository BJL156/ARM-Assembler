#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void program_append(Program *program, Stmt stmt) {
  if (program->count == program->capacity) {
    program->capacity = program->capacity == 0 ? 8 : program->capacity * 2;
    program->stmts = realloc(program->stmts, program->capacity * sizeof(Stmt));
  }
  program->stmts[program->count++] = stmt;
}

void program_free(Program *program) {
  free(program->stmts);
  program->stmts = NULL;
  program->count = 0;
  program->capacity = 0;
}

Stmt parse_directive(Lexer *lexer, Token first) {
  Stmt stmt;
  memset(&stmt, 0, sizeof(stmt));
  stmt.type = STMT_DIRECTIVE;
  stmt.line = first.line;
  strncpy(stmt.directive.name, first.str, sizeof(stmt.directive.name) - 1);

  Token arg = next_token(lexer);
  if (arg.type == TOKEN_LABEL_REF ||
      arg.type == TOKEN_MNEMONIC ||
      arg.type == TOKEN_STRING) {
    strncpy(stmt.directive.arg, arg.str, sizeof(stmt.directive.arg) - 1);
  } else if (arg.type == TOKEN_IMM) {
    stmt.directive.arg_imm = arg.imm;
  }

  return stmt;
}

Stmt parse_instr(Lexer *lexer, Token first) {
  Stmt stmt;
  memset(&stmt, 0, sizeof(stmt));
  stmt.type = STMT_INSTR;
  stmt.line = first.line;
  strncpy(stmt.instr.mnemonic, first.str, sizeof(stmt.instr.mnemonic) - 1);

  int i = 0;
  while (i < 4) {
    Token token = next_token(lexer);

    if (token.type == TOKEN_NEWLINE || token.type == TOKEN_EOF) {
      break;
    }

    if (token.type == TOKEN_UNKNOWN) {
      continue;
    }

    Operand op;
    memset(&op, 0, sizeof(op));

    if (token.type == TOKEN_REG) {
      op.type = OP_REG;
      op.reg = token.reg;
      op.is_32bit = token.is_32bit;
    } else if (token.type == TOKEN_IMM) {
      op.type = OP_IMM;
      op.imm = token.imm;
    } else if (token.type == TOKEN_LABEL_REF) {
      op.type = OP_LABEL;
      strncpy(op.label, token.str, sizeof(op.label) - 1);
    } else if (token.type == TOKEN_LBRACKET) {
      Token base = next_token(lexer);
      if (base.type != TOKEN_REG) {
        fprintf(stderr, "Error: expected register after left bracket at line: %d.\n", base.line);
        break;
      }

      op.type = OP_MEM;
      op.mem.base_reg = base.reg;
      op.mem.offset = 0;
      op.mem.mode = MEM_OFFSET;

      Token next = next_token(lexer);

      if (next.type == TOKEN_UNKNOWN) {
        Token imm = next_token(lexer);
        if (imm.type == TOKEN_IMM) {
          op.mem.offset = imm.imm;
        }
        next = next_token(lexer); // should be ']'
      }

      if (next.type != TOKEN_RBRACKET) {
        fprintf(stderr, "Error: expected right bracket at line: %d.\n", next.line);
        break;
      }

      Token after = next_token(lexer);
      if (after.type == TOKEN_UNKNOWN && after.str[0] == '!') {
        op.mem.mode = MEM_PRE_INDEX;
      } else if (after.type == TOKEN_UNKNOWN && after.str[0] == ',') {
        Token post_imm = next_token(lexer);
        if (post_imm.type == TOKEN_IMM) {
          op.mem.offset = post_imm.imm;
          op.mem.mode = MEM_POST_INDEX;
        }
      }
    } else if (token.type == TOKEN_MNEMONIC && strcasecmp(token.str, "lsl") == 0) {
      Token shift_imm = next_token(lexer);
      if (shift_imm.type != TOKEN_IMM) {
        fprintf(stderr, "Error: expected immediate after lsl at line: %d.\n", shift_imm.line);
        break;
      }

      op.type = OP_IMM;
      op.imm = shift_imm.imm;
    } else {
      fprintf(stderr, "Error: unexpected token in operand at line %d.\n", token.line);
      break;
    }

    stmt.instr.operands[i++] = op;
  }

  stmt.instr.operand_count = i;
  return stmt;
}

void parse(Lexer *lexer, Program *program) {
  program->stmts = NULL;
  program->capacity = 0;
  program->count = 0;

  for (;;) {
    Token token = next_token(lexer);

    if (token.type == TOKEN_EOF) {
      break;
    }

    if (token.type == TOKEN_NEWLINE ||
        token.type == TOKEN_UNKNOWN) {
      continue;
    }

    if (token.type == TOKEN_LABEL_DEF) {
      Stmt stmt;
      memset(&stmt, 0, sizeof(stmt));
      stmt.type = STMT_LABEL;
      stmt.line = token.line;
      strncpy(stmt.label, token.str, sizeof(stmt.label) - 1);
      program_append(program, stmt);
      continue;
    }

    if (token.type == TOKEN_DIRECTIVE) {
      program_append(program, parse_directive(lexer, token));
      continue;
    }

    if (token.type == TOKEN_MNEMONIC) {
      program_append(program, parse_instr(lexer, token));
      continue;
    }

    fprintf(stderr, "Error: unexpected token \"%s\" at line: %d.\n", token.str, token.line);
  }
}
