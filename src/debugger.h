#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "cpu.h"

void debugger_run(CPU *cpu, Instr *instructions, int number_of_instruction);

#endif