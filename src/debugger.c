#include "debugger.h"

#include <stdio.h>
#include <string.h>

#include "ram.h"

#define LINE_SIZE 16

static const char *const flag_names[] = {[ZF] = "ZF"};

static void print_regs(CPU *cpu) {
    int quarter = REG_COUNT >> 2;
    for (int i = 0; i < quarter; i++) {
        for (int j = 0; j < quarter; j++) {
            printf("[%s] = %lld\t", reg_names[quarter * j + i], (long long)cpu->regs[quarter * j + i]);
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
static void print_memory(const Memory *mem) {

    for (int i = 0; i < MEMSIZE; i++) {
        if (mem->cells[i] != 0) {
            printf("MEMORY[%d] = %lld\n", i, (long long)mem->cells[i]);
        }
    }
}

static void print_help(void) {
    printf("Commands:\nr - REG info\nf - FLAGS info\nm - MEMORY\nc - execute till end\ns or Enter - next "
           "step\nq - quit\nh - help\n");
}
void debugger_run(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;
    int till_end = 0;
    while (cpu->pc < number_of_instruction && !cpu->is_halted && cpu->error != 1) {
        instr = instructions + cpu->pc;
        if (till_end != 1) {
            printf("%d ", cpu->pc);
            print_instr(stdout, instr);
            printf("\n");
        }
        int step = 0;

        while (!step && !till_end) {

            char command[LINE_SIZE];
            printf("> ");
            if (fgets(command, sizeof(command), stdin) == NULL) {
                till_end = 1;
                break;
            }

            if (strchr(command, '\n') == NULL) {
                printf("Too long command\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }
                continue;
            }

            switch (command[0]) {
            case 'r':
                print_regs(cpu);
                break;
            case 'f':
                print_flags(cpu);
                break;
            case 'm':
                print_memory(cpu->mem);
                break;
            case 'q':
                return;
            case 'h':
                print_help();
                break;
            case 'c':
                till_end = 1;
                break;
            case '\n':
            case 's':
                step = 1;
                break;
            default:
                printf("Unknown command, type h for help");
                break;
            }

            printf("\n");
        }
        execute_one(cpu, instr);
    }
    if (cpu->error == 1) {
        printf("Stopped: runtime error\n");
    } else if (cpu->is_halted == 1) {
        printf("Program halted\n");
    } else {
        printf("Reached the end of the program\n");
    }
}
