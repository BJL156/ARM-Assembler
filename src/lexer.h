#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

typedef enum {
  TOKEN_MNEMONIC,
  TOKEN_REG,
  TOKEN_IMM,
  TOKEN_LABEL_DEF,
  TOKEN_LABEL_REF,
  TOKEN_DIRECTIVE,
  TOKEN_NEWLINE,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_EOF,
  TOKEN_STRING,
  TOKEN_UNKNOWN
} TokenType;

typedef struct {
  TokenType type;
  union {
    char str[64];
    int reg;
    int64_t imm;
  };
  int line;
  int col;
} Token;

typedef struct {
  const char *src;
  int pos;
  int line;
  int col;
} Lexer;

void lexer_init(Lexer *lexer, const char *src);
Token next_token(Lexer *lexer);

#endif
