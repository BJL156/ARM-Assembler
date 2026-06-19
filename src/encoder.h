#ifndef ENCODER_H
#define ENCODER_H

#include "parser.h"
#include "symtab.h"

#include <stdint.h>

uint32_t encode_instr(Stmt *stmt, uint32_t pc, SymTab *symtab);

#endif
