#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "cpu.h"
#include "debugger.h"
void print_help(void) {
    printf("Options:\n");
    printf("%-26s %s\n", "-h, --help", "Display this help message");
    printf("%-26s %s\n", "-r \"file.asm\"", "run <file.asm> non-interactively (batch mode)");
    printf("%-26s %s\n", "-d \"file.asm\"", "run <file.asm> in interactive debugger mode");
}

int main(int argc, char *argv[]) {
    int number_of_instruction;
    int is_for_debugging = -1;
    Instr *instr_list = NULL;
    if (argc == 1) {
        fprintf(stderr, "Please, use options\n");
        print_help();
        return 1;
    } else if (argc == 2) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            print_help();
            return 0;
        } else {
            fprintf(stderr, "No command was found\n");
            print_help();
            return 1;
        }
    } else if (argc == 3) {

        if (!strcmp(argv[1], "-r")) {
            is_for_debugging = 0;
        } else if (!strcmp(argv[1], "-d")) {
            is_for_debugging = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            return 1;
        }
        instr_list = asembler(argv[2], &number_of_instruction);

    } else {
        fprintf(stderr, "Please, use options\n");
        print_help();
        return 1;
    }

    if (instr_list == NULL) {

        return 1;
    }
    CPU processor;
    CPU *cpu = cpu_init(&processor);

    if (is_for_debugging == 0) {
        cpu_run(cpu, instr_list, number_of_instruction);
    } else if (is_for_debugging == 1) {
        debuger(cpu, instr_list, number_of_instruction);
    }

    free(instr_list);
    if(cpu->err_exit == 1) return 1;
    return 0;
}
