#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "cpu.h"
#include "debugger.h"

int main(int argc, char *argv[]) {
    int file_given = 0;
    int is_for_compiling_debuging = 0;
    char name_of_file[LINE_BUFFER_SIZE] = {0};
    if (argc > 3) {
        if (!strcmp(argv[1], "-c") && !strcmp(argv[2], "-f") && (strlen(argv[3]) < LINE_BUFFER_SIZE)) {
            strcpy(name_of_file, argv[3]);
            file_given = 1;
            is_for_compiling_debuging = 1;
        } else if (!strcmp(argv[1], "-d") && !strcmp(argv[2], "-f") && (strlen(argv[3]) < LINE_BUFFER_SIZE)) {
            strcpy(name_of_file, argv[3]);
            file_given = 1;
            is_for_compiling_debuging = 2;
        }
    }

    int number_of_instruction;

    Instr *instr_list = asembler(file_given ? name_of_file : "program.asm", &number_of_instruction);
    if (instr_list == NULL)
        return 0;
    CPU processor;
    CPU *cpu = cpu_init(&processor);
    if (is_for_compiling_debuging == 1) {
        compiler(cpu, instr_list, number_of_instruction);
    } else if (is_for_compiling_debuging == 2) {
        debuger(cpu, instr_list, number_of_instruction);
    } else {
        debuger(cpu, instr_list, number_of_instruction);
    }

    free(instr_list);

    return 0;
}
