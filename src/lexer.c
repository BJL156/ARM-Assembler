#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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

Token scan_eof(Token *token) {
  token->type = TOKEN_EOF;
  return *token;
}

Token scan_newline(Lexer *lexer, Token *token) {
  advance(lexer);
  token->type = TOKEN_NEWLINE;
  return *token;
}

Token scan_directive(Lexer *lexer, Token *token) {
  advance(lexer);
  int i = 0;
  while (isalnum(peek(lexer)) || peek(lexer) == '_') {
    token->str[i++] = advance(lexer);
  }

  token->str[i] = '\0';
  token->type = TOKEN_DIRECTIVE;
  return *token;
}

Token scan_immediate(Lexer *lexer, Token *token) {
  advance(lexer);

  int64_t value = 0;
  bool is_negative = false;

  if (peek(lexer) == '-') {
    is_negative = true;
    advance(lexer);
  }

  if (peek(lexer) == '0' && (peek_next(lexer) == 'x' || peek_next(lexer) == 'X')) {
    advance(lexer);
    advance(lexer);

    while (isxdigit(peek(lexer))) {
      char digit = advance(lexer);
      value = value * 16 + (isdigit(digit) ? digit - '0' : tolower(digit) - 'a' + 10);
    }
  } else {
    while (isdigit(peek(lexer))) {
      char digit = advance(lexer);
      value = value * 10 + (digit - '0');
    }
  }

  token->type = TOKEN_IMM;
  token->imm = is_negative ? -value : value;
  return *token;
}

bool try_scan_register(Token *token, char *str) {
  if (str[0] == 'x' || str[0] == 'X') {
    if (strcmp(str + 1, "zr") == 0) {
      token->type = TOKEN_REG;
      token->reg = 31;
      return true;
    }

    char *end;
    long num = strtol(str + 1, &end, 10);
    if (*end == '\0' && num >= 0 && num <= 30) {
      token->type = TOKEN_REG;
      token->reg = (int)num;
      return true;
    }
  }

  if (strcmp(str, "sp") == 0 || strcmp(str, "SP") == 0) {
    token->type = TOKEN_REG;
    token->reg = 31;
    return true;
  }

  return false;
}

bool is_mnemonic(const char *str) {
  const char *mnemonics[] = {
    "mov", "add", "sub", "svc", "b", "bl", "ret", "ldr", "str", "cmp", "cbz", "cbnz", "b.eq", "b.ne", "nop", "adr", "ldrb", "strb",  "mul", NULL
  };

  for (int i = 0; mnemonics[i]; i++) {
    if (strcasecmp(str, mnemonics[i]) == 0) {
      return true;
    }
  }

  return false;
}

Token scan_identifier(Lexer *lexer, Token *token) {
  int i = 0;
  while (isalnum(peek(lexer)) || peek(lexer) == '_' || (peek(lexer) == '.' && isalpha(peek_next(lexer)))) {
    token->str[i++] = advance(lexer);
  }
  token->str[i] = '\0';

  if (peek(lexer) == ':') {
    advance(lexer);
    token->type = TOKEN_LABEL_DEF;
    return *token;
  }

  if (try_scan_register(token, token->str)) {
    return *token;
  }

  if (is_mnemonic(token->str)) {
    token->type = TOKEN_MNEMONIC;
    return *token;
  }

  token->type = TOKEN_LABEL_REF;
  return *token;
}

Token scan_string(Lexer *lexer, Token *token) {
  advance(lexer);

  int i = 0;
  while (peek(lexer) != '"' && peek(lexer) != '\0' && i < 63) {
    char c = advance(lexer);
    if (c == '\\') {
      char escape_code = advance(lexer);
      switch (escape_code) {
        case 'n':  token->str[i++] = '\n'; break;
        case 't':  token->str[i++] = '\t'; break;
        case '\\': token->str[i++] = '\\'; break;
        case '"':  token->str[i++] = '"'; break;
        case '0':  token->str[i++] = '\0'; break;
        default:   token->str[i++] = escape_code; break;
      }
    } else {
      token->str[i++] = c;
    }
  }

  if (peek(lexer) == '"') {
    advance(lexer);
  }

  token->str[i] = '\0';
  token->type = TOKEN_STRING;
  return *token;
}

Token next_token(Lexer *lexer) {
  skip_whitespace(lexer);

  Token token;
  memset(&token, 0, sizeof(token));
  token.line = lexer->line;
  token.col = lexer->col;

  char c = peek(lexer);

  if (c == '\0') return scan_eof(&token);
  if (c == '\n') return scan_newline(lexer, &token);
  if (c == '.')  return scan_directive(lexer, &token);
  if (c == '#')  return scan_immediate(lexer, &token);
  if (c == '"')  return scan_string(lexer, &token);
  
  if (c == '[') {
    advance(lexer);
    token.type = TOKEN_LBRACKET;
    return token;
  }

  if (c == ']') {
    advance(lexer);
    token.type = TOKEN_RBRACKET;
    return token;
  }

  if (isalpha(c) || c == '_') {
    return scan_identifier(lexer, &token);
  }

  advance(lexer);
  token.type = TOKEN_UNKNOWN;
  token.str[0] = c;
  token.str[1] = '\0';
  return token;
}
