#ifndef ISA_H
#define ISA_H

#include <stdint.h>

#define LABEL_NAME_SIZE 50

typedef enum { MOV, ADD, DEC, INC, SUB, CMP, JMP, JE, HALT, PRINT, OPCODE_COUNT } Opcode;
typedef enum { NONE, MEM_ADDR, VALUE, REG, LABEL } OperandType;
typedef enum { RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15, REG_COUNT } Reg;

typedef struct {
    OperandType type;
    int64_t value;
    char label[LABEL_NAME_SIZE];
} Operand;

typedef struct {
    Opcode opcode;
    Operand operand1;
    Operand operand2;
} Instr;

extern const char *const opcode_names[OPCODE_COUNT];
extern const char *const reg_names[REG_COUNT];

#endif
