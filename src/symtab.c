#include "symtab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void symtab_init(SymTab *symtab) {
  symtab->symbols = NULL;
  symtab->count = 0;
  symtab->capacity = 0;
}

void symtab_free(SymTab *symtab) {
  free(symtab->symbols);
  symtab->symbols = NULL;
  symtab->count = 0;
  symtab->capacity = 0;
}

void symtab_define(SymTab *symtab, const char *name, uint32_t addr) {
  for (int i = 0; i < symtab->count; i++) {
    if (strcmp(symtab->symbols[i].name, name) == 0) {  
      fprintf(stderr, "Error: label \"%s\" has been defined multiple times.\n", name);
      return;
    }
  }

  if (symtab->count == symtab->capacity) {
    symtab->capacity = symtab->capacity == 0 ? 8 : symtab->capacity * 2;
    symtab->symbols = realloc(symtab->symbols, symtab->capacity * sizeof(Symbol));
  }

  strncpy(symtab->symbols[symtab->count].name, name, 63);
  symtab->symbols[symtab->count].name[63] = '\0';
  symtab->symbols[symtab->count].addr = addr;
  symtab->count++;
}

bool symtab_lookup(SymTab *symtab, const char *name, uint32_t *addr) {
  for (int i = 0; i < symtab->count; i++) {
    if (strcmp(symtab->symbols[i].name, name) == 0) {
      *addr = symtab->symbols[i].addr;
      return true;
    }
  }

  return false;
}
