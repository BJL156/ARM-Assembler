#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char name[64];
  uint32_t addr;
} Symbol;

typedef struct {
  Symbol *symbols;
  int count;
  int capacity;
} SymTab;

void symtab_init(SymTab *symtab);
void symtab_free(SymTab *symtab);
void symtab_define(SymTab *symtab, const char *name, uint32_t addr);
bool symtab_lookup(SymTab *symtab, const char *name, uint32_t *addr);

#endif
