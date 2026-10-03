#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "isa.h"

Instr *asembler(const char *file_name, int *number_of_instructions);

#endif