#include "cpu.h"

#include <stdio.h>
#include <string.h>

#include "ram.h"



void execute_one(CPU *cpu, Instr *instr) {

    int jumped = 0;
    if (instr->opcode == MOV) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] = cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] = instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] = cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] = instr->operand2.value;
            }
        }
    } else if (instr->opcode == ADD) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] += cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] += instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] += cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] += instr->operand2.value;
            }
        }
    } else if (instr->opcode == SUB) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] -= instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] -= instr->operand2.value;
            }
        }
    } else if (instr->opcode == DEC) {
        if (instr->operand1.type == REG) {
            cpu->REGS[instr->operand1.value]--;
        } else if (instr->operand1.type == MEM_ADDR) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value]--;
            }
        }
    } else if (instr->opcode == INC) {
        if (instr->operand1.type == REG) {
            cpu->REGS[instr->operand1.value]++;
        } else if (instr->operand1.type == MEM_ADDR) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value]++;
            }
        }
    } else if (instr->opcode == CMP) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            if (cpu->REGS[instr->operand1.value] - cpu->REGS[instr->operand2.value] == 0)
                cpu->flag[ZF] = 1;
            else
                cpu->flag[ZF] = 0;
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            if (cpu->REGS[instr->operand1.value] - instr->operand2.value == 0)
                cpu->flag[ZF] = 1;
            else
                cpu->flag[ZF] = 0;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                if (MEMORY[instr->operand1.value] - cpu->REGS[instr->operand2.value] == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                if (MEMORY[instr->operand1.value] - instr->operand2.value == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            }
        }

    } else if (instr->opcode == JE && cpu->flag[ZF]) {
        cpu->pc = (int)instr->operand1.value;
        jumped = 1;
    } else if (instr->opcode == JMP) {
        cpu->pc = (int)instr->operand1.value;
        jumped = 1;

    } else if (instr->opcode == PRINT) {
        if (instr->operand1.type == REG) {
            printf("%lld\n", cpu->REGS[instr->operand1.value]);
        } else if (instr->operand1.type == MEM_ADDR && is_valid_mem_addr(instr->operand1.value)) {
            printf("%llu\n", MEMORY[instr->operand1.value]);
        } else if (instr->operand1.type == VALUE) {
            printf("%lld\n", instr->operand1.value);
        }

    } else if (instr->opcode == HALT) {
        cpu->is_halted = 1;
    }

    if (!jumped) {
        cpu->pc++;
    }
}

CPU *cpu_init(CPU *processor) {

    CPU *cpu = processor;

    memset(cpu, 0, sizeof(*cpu));

    return cpu;
}

void cpu_run(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction && !cpu->is_halted) {
        instr = instructions + cpu->pc;
        execute_one(cpu, instr);
    }
}
