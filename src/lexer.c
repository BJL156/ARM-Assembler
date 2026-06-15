#include "lexer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
  const char *src;
  int pos;
  int line;
  int col;
} Lexer;

void lexer_init(Lexer *lexer, const char *src) {
  lexer->src = src;
  lexer->pos = 0;
  lexer->line = 1;
  lexer->col = 1;
}

char peek(Lexer *lexer) {
  return lexer->src[lexer->pos];
}

char peek_next(Lexer *lexer) {
  return lexer->src[lexer->pos + 1];
}

char advance(Lexer *lexer) {
  char c = lexer->src[lexer->pos++];
  if (c == '\n') {
    lexer->line++;
    lexer->col = 1;
  } else {
    lexer->col++;
  }

  return c;
}

void skip_whitespace(Lexer *lexer) {
  for (;;) {
    while (peek(lexer) == ' ' || peek(lexer) == '\t') {
      advance(lexer);
    }

    if ((peek(lexer) == '/' && peek_next(lexer) == '/') ||
        (peek(lexer) == ';') ||
        (peek(lexer) == '@')) {
      while (peek(lexer) != '\n' && peek(lexer) != '\0') {
        advance(lexer);
      }
    } else {
      break;
    }
  }
}
