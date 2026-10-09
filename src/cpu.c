#include "cpu.h"

#include <stdio.h>
#include <string.h>

#include "ram.h"

static int64_t op_to_val(CPU *cpu, Operand *op) {
    int64_t val = 0;
    if (op->type == MEM_ADDR) {
        if (is_valid_mem_addr(op->value))
            val = cpu->mem->cells[op->value];
        else {
            fprintf(stderr, "Memory access out of bounds at address %lld\n", op->value);
            cpu->error = 1;
        }

    } else if (op->type == VALUE) {
        val = op->value;
    } else if (op->type == REG) {
        val = cpu->regs[op->value];
    } else {
        fprintf(stderr, "Operand has to be Reg or Memory or Value type\n");
        cpu->error = 1;
    }

    return val;
}
static int64_t *op_to_addr(CPU *cpu, Operand *op) {
    int64_t *val = NULL;
    if (op->type == MEM_ADDR) {
        if (is_valid_mem_addr(op->value))
            val = &cpu->mem->cells[op->value];
        else {
            fprintf(stderr, "Memory access out of bounds at address %lld\n", op->value);
            cpu->error = 1;
        }
    } else if (op->type == REG) {
        val = &cpu->regs[op->value];
    } else {
        fprintf(stderr, "First Operand has to be Reg or Memory type\n");
        cpu->error = 1;
    }
    return val;
}

void execute_one(CPU *cpu, Instr *instr) {

    int jumped = 0;

    switch (instr->opcode) {
    case MOV:
    case ADD:
    case SUB: {
        int64_t *first_operand_addr = op_to_addr(cpu, &instr->operand1);
        int64_t second_operand = op_to_val(cpu, &instr->operand2);
        if (cpu->error)
            return;

        if (instr->opcode == MOV)
            *first_operand_addr = second_operand;
        else if (instr->opcode == ADD)
            *first_operand_addr += second_operand;
        else if (instr->opcode == SUB)
            *first_operand_addr -= second_operand;

        break;
    }
    case DEC:
    case INC: {
        int64_t *first_operand_addr = op_to_addr(cpu, &instr->operand1);

        if (cpu->error)
            return;

        if (instr->opcode == INC)
            (*first_operand_addr)++;
        else if (instr->opcode == DEC)
            (*first_operand_addr)--;    

        break;
    }
    case CMP: {
        cpu->flag[ZF] = (op_to_val(cpu, &instr->operand1)) == op_to_val(cpu, &instr->operand2);
        break;
    }
    case JE: {
        if (cpu->flag[ZF]) {
            cpu->pc = (int)instr->operand1.value;
            jumped = 1;
        }
        break;
    }
    case JMP: {
        cpu->pc = (int)instr->operand1.value;
        jumped = 1;
        break;
    }
    case PRINT: {
        int64_t to_print = op_to_val(cpu, &instr->operand1);
        if (cpu->error != 1) {
            printf("%lld\n", to_print);
        }
        break;
    }
    case HALT: {
         cpu->is_halted = 1;
        break;
    }
    default: 
        fprintf(stderr, "invalid opcode %d at pc %d\n", instr->opcode, cpu->pc);;
        cpu->error = 1;
        return;
    }
   

    if (!jumped) {
        cpu->pc++;
    }
}

CPU *cpu_init(CPU *processor, Memory *memory) {

    CPU *cpu = processor;

    memset(cpu, 0, sizeof(*cpu));

    cpu->mem = memory;

    return cpu;
}

void cpu_run(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr;

    while (cpu->pc < number_of_instruction && !cpu->is_halted && cpu->error != 1) {
        instr = instructions + cpu->pc;
        execute_one(cpu, instr);
    }
}
