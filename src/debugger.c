#include "debugger.h"

#include <stdio.h>

#include "ram.h"

static const char *const flag_names[] = {[ZF] = "ZF"};

static void print_regs(CPU *cpu) {
    int quarter = (sizeof(reg_names) / sizeof(reg_names[0])) >> 2;
    for (int i = 0; i < quarter; i++) {
        for (int j = 0; j < quarter; j++) {
            printf("[%s] = %lld\t", reg_names[quarter * j + i], cpu->regs[quarter * j + i]);
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

void debuger_run(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction && !cpu->is_halted && cpu->error != 1) {
        instr = instructions + cpu->pc;
        int executed_pc = cpu->pc;
        execute_one(cpu, instr);
        int choice = -1;
        printf("What you want to see:\n1 - REG info\n2 - FLAGS info\n3 - MEMORY\n4 - pc\n");
        while ((choice = getchar()) != '\n' && choice != EOF) {
            if (choice == '0') {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }
                break;
            } else if (choice == '1') {
                print_regs(cpu);
            } else if (choice == '2') {
                print_flags(cpu);
            } else if (choice == '3') {
                print_memory();
            } else if (choice == '4') {
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
