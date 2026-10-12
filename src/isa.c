#include "isa.h"
#include <stdio.h>

const char *const reg_names[] = {[RBX] = "RBX", [RCX] = "RCX", [RDX] = "RDX", [RSI] = "RSI",
                                 [RDI] = "RDI", [RBP] = "RBP", [RSP] = "RSP", [RAX] = "RAX",
                                 [R8] = "R8",   [R9] = "R9",   [R10] = "R10", [R11] = "R11",
                                 [R12] = "R12", [R13] = "R13", [R14] = "R14", [R15] = "R15"};

const char *const opcode_names[] = {[MOV] = "MOV", [ADD] = "ADD", [DEC] = "DEC", [INC] = "INC",   [SUB] = "SUB",
                                    [CMP] = "CMP", [JMP] = "JMP", [JE] = "JE",   [HALT] = "HALT", [PRINT] = "PRINT"};

static void print_operand(FILE *out, const Operand *op) {
    switch (op->type) {
    case REG:
        fprintf(out, "%s", reg_names[op->value]);
        break;
    case VALUE:
        fprintf(out, "%lld", (long long)op->value);
        break;
    case MEM_ADDR:
        fprintf(out, "[%lld]", (long long)op->value);
        break;
    case LABEL:
        fprintf(out, "%s", op->label);
        break;
    case NONE:
        break;
    }
}

void print_instr(FILE *out, const Instr *instr) {
    fprintf(out, "%s", opcode_names[instr->opcode]);

    if (instr->operand1.type != NONE) {
        fprintf(out, " ");
        print_operand(out, &instr->operand1);
    }
    if (instr->operand2.type != NONE) {
        fprintf(out, ", ");
        print_operand(out, &instr->operand2);
    }
}
