#ifndef CPU_H
#define CPU_H

#include "isa.h"

#define FLAG_COUNTER 1

typedef enum { ZF } Flag_enum;

typedef struct {

    int64_t REGS[16];
    int flag[FLAG_COUNTER];
    int pc;
    int is_halted;

} CPU;

CPU *cpu_init(CPU *processor);
void execute_one(CPU *cpu, Instr *instr);
void cpu_run(CPU *cpu, Instr *instructions, int number_of_instruction);

#endif