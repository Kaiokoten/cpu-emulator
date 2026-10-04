#include "debugger.h"

#include <stdio.h>

#include "ram.h"

static const char *const flag_names[] = {[ZF] = "ZF"};

static void print_regs(CPU *cpu) {
    int quater = (sizeof(reg_names) / sizeof(reg_names[0])) >> 2;
    for (int i = 0; i < quater; i++) {
        for (int j = 0; j < quater; j++) {
            printf("[%s] = %lld\t", reg_names[quater * j + i], cpu->REGS[quater * j + i]);
        }
        printf("\n");
    }
}
static void print_flags(CPU *cpu) {
    int flags_s = sizeof(cpu->flag) / sizeof(cpu->flag[0]);
    for (int i = 0; i < flags_s; i++) {
        printf("[%s] = %d\t", flag_names[i], cpu->flag[i]);
    }
    printf("\n");
}
static void print_memory(void) {

    char bit_number[sizeof(MEMORY[0]) * 8 + 1];
    bit_number[sizeof(MEMORY[0]) * 8] = '\0';
    for (int i = 0; i < MEMSIZE; i++) {
        if (MEMORY[i] != 0) {
            for (size_t j = 0; j < sizeof(MEMORY[0]) * 8; j++) {
                bit_number[sizeof(MEMORY[0]) * 8 - 1 - j] = '0' + ((MEMORY[i] >> j) & 1);
            }
            printf("MEMORY[%d] = %s", i, bit_number);
        }
    }
}

void debuger(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction && !cpu->is_halted && cpu->err_exit != 1) {
        instr = instructions + cpu->pc;
        int executed_pc = cpu->pc;
        execute_one(cpu, instr);
        int choise = -1;
        printf("What you want to see:\n1 - REG info\n2 - FLAGS info\n3 - MEMORY\n4 - pc\n");
        while ((choise = getchar()) != '\n' && choise != EOF) {
            if (choise == '0') {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }
                break;
            } else if (choise == '1') {
                print_regs(cpu);
            } else if (choise == '2') {
                print_flags(cpu);
            } else if (choise == '3') {
                print_memory();
            } else if (choise == '4') {
                printf("%d - command [%s]", executed_pc, opcode_names[instr->opcode]);
            }

            printf("\n");
            printf("What you want to see:\n1 - REG info\n2 - FLAGS info\n3 - MEMORY\n4 - pc\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
        }
    }
}
