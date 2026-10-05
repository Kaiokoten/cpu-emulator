#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "isa.h"

#define COMMENT_SYMBOL '#'
Instr *assemble(const char *file_name, int *number_of_instructions);

#endif