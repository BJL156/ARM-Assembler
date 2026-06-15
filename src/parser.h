#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#include <stdint.h>

typedef enum {
  OP_REG,
  OP_IMM,
  OP_LABEL
} OperandType;

typedef struct {
  OperandType type;
  union {
    int reg;
    int64_t imm;
    char label[64];
  };
} Operand;

typedef enum {
  STMT_LABEL,
  STMT_DIRECTIVE,
  STMT_INSTR
} StmtType;

typedef struct {
  StmtType type;
  union {
    char label[64];
    struct {
      char name[64];
      char arg[64];
    }   directive;
    struct {
      char mnemonic[64];
      Operand operands[3];
      int operand_count;
    } instr;
  };
  int line;
} Stmt;

typedef struct {
  Stmt *stmts;
  int count;
  int capacity;
} Program;

void parse(Lexer *lexer, Program *program);
void program_free(Program *program);

#endif
