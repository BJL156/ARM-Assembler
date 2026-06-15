#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

typedef enum {
  TOK_MNEMONIC,
  TOK_REG,
  TOK_IMM,
  TOK_LABEL_DEF,
  TOK_LABEL_REF,
  TOK_DIRECTIVE,
  TOK_NEWLINE,
  TOK_EOF,
  TOK_UNKNOWN
} TokenType;

typedef struct {
  TokenType token_type;
  union {
    char str[64];
    int reg;
    int64_t imm;
  };
  int line;
  int col;
} Token;

#endif
