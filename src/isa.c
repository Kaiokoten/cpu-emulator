#include "isa.h"

const char *const reg_names[] = {[RBX] = "RBX", [RCX] = "RCX", [RDX] = "RDX", [RSI] = "RSI",
                                 [RDI] = "RDI", [RBP] = "RBP", [RSP] = "RSP", [RAX] = "RAX",
                                 [R8] = "R8",   [R9] = "R9",   [R10] = "R10", [R11] = "R11",
                                 [R12] = "R12", [R13] = "R13", [R14] = "R14", [R15] = "R15"};

const char *const opcode_names[] = {[MOV] = "mov", [ADD] = "add", [DEC] = "dec", [INC] = "inc",   [SUB] = "sub",
                                    [CMP] = "cmp", [JMP] = "jmp", [JE] = "je",   [HALT] = "halt", [PRINT] = "print"};
