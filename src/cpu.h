#ifndef CPU_H
#define CPU_H

#include "isa.h"
#include "ram.h"

#define FLAG_COUNTER 1

typedef enum { ZF } Flag_enum;

typedef struct {

    int64_t regs[REG_COUNT];
    int flag[FLAG_COUNTER];
    int pc;
    int is_halted;
    int error;

    Memory *mem;

} CPU;

CPU *cpu_init(CPU *processor, Memory *mem);
void execute_one(CPU *cpu, Instr *instr);
void cpu_run(CPU *cpu, Instr *instructions, int number_of_instruction);

#endif