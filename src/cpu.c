#include "cpu.h"

#include <stdio.h>
#include <string.h>

#include "ram.h"

static int64_t op_to_val(CPU *cpu, Operand *op) {
    int64_t val = 0;
    if (op->type == MEM_ADDR) {
        if (op->value < MEMSIZE && op->value >= 0)
            val = MEMORY[op->value];
        else {
            fprintf(stderr, "Memory access out of bounds at address %lld\n", op->value);
            cpu->err_exit = 1;
        }

    } else if (op->type == VALUE) {
        val = op->value;
    } else if (op->type == REG) {
        val = cpu->REGS[op->value];
    } else {
        fprintf(stderr, "Operand has to be Reg or Memory or Value type\n");
        cpu->err_exit = 1;
    }

    return val;
}
static int64_t *op_to_addr(CPU *cpu, Operand *op) {
    int64_t *val = NULL;
    if (op->type == MEM_ADDR) {
        if (op->value < MEMSIZE && op->value >= 0)
            val = &MEMORY[op->value];
        else {
            fprintf(stderr, "Memory access out of bounds at address %lld\n", op->value);
            cpu->err_exit = 1;
        }
    } else if (op->type == REG) {
        val = &cpu->REGS[op->value];
    } else {
        fprintf(stderr, "First Operand has to be Reg or Memory type\n");
        cpu->err_exit = 1;
    }
    return val;
}

void execute_one(CPU *cpu, Instr *instr) {

    int jumped = 0;
    if (instr->opcode == MOV || instr->opcode == ADD || instr->opcode == SUB) {
        int64_t *first_operand_addr = op_to_addr(cpu, &instr->operand1);
        int64_t second_operand = op_to_val(cpu, &instr->operand2);
        if (cpu->err_exit != 1) {
            if (instr->opcode == MOV)
                *first_operand_addr = second_operand;
            else if (instr->opcode == ADD)
                *first_operand_addr += second_operand;
            else if (instr->opcode == SUB)
                *first_operand_addr -= second_operand;
        } else
            return;
    }

    else if (instr->opcode == DEC || instr->opcode == INC) {
        int64_t *first_operand_addr = op_to_addr(cpu, &instr->operand1);
        if (cpu->err_exit != 1) {
            if (instr->opcode == INC)
                (*first_operand_addr)++;
            else if (instr->opcode == DEC)
                (*first_operand_addr)--;
        } else
            return;

    } else if (instr->opcode == CMP) {
        cpu->flag[ZF] = !((op_to_val(cpu, &instr->operand1)) - op_to_val(cpu, &instr->operand2));

    } else if (instr->opcode == JE && cpu->flag[ZF]) {
        cpu->pc = (int)instr->operand1.value;
        jumped = 1;
    } else if (instr->opcode == JMP) {
        cpu->pc = (int)instr->operand1.value;
        jumped = 1;

    } else if (instr->opcode == PRINT) {

        int64_t to_print = op_to_val(cpu, &instr->operand1);
        if (cpu->err_exit != 1) {
            printf("%lld\n", to_print);
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

    while (cpu->pc < number_of_instruction && !cpu->is_halted && cpu->err_exit != 1) {
        instr = instructions + cpu->pc;
        execute_one(cpu, instr);
    }
}
