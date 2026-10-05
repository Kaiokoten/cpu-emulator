#ifndef ISA_H
#define ISA_H

#include <stdint.h>

#define OPCODE_COUNT 10
#define REG_COUNT 16
#define LABEL_SIZE 50

#define LINE_BUF_SIZE 150
#define INSTRUCTION_BUF_SIZE 100
#define LABEL_BUF_SIZE 50

typedef enum { MOV, ADD, DEC, INC, SUB, CMP, JMP, JE, HALT, PRINT } Opcode;
typedef enum { MEM_ADDR, VALUE, REG, LABEL } Type_of_operand;
typedef enum { RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15 } Reg;

typedef struct {
    Type_of_operand type;
    int64_t value;
    char label[LABEL_SIZE];
} Operand;

typedef struct {
    Opcode opcode;
    Operand operand1;
    Operand operand2;
    Operand operand3;
} Instr;

extern const char *const opcode_names[OPCODE_COUNT];
extern const char *const reg_names[REG_COUNT];

#endif
