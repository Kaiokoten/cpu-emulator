#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 50
#define MEMSIZE 4092
#define INSTRUCTION_BUF 100
#define FLAG_COUNTER 2
#define LABEL_BUF_SIZE 50
uint64_t MEMORY[MEMSIZE] = {0};

typedef enum {
    MOV,
    ADD,
    DEC,
    INC,
    SUB,
    CMP,
    JMP
} Opcode;

const char *opcode_names[] = {[MOV] = "mov", [ADD] = "add", [DEC] = "dec", [INC] = "inc",
                              [SUB] = "sub", [CMP] = "cmp", [JMP] = "jmp"

};

typedef enum {
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15
} Reg;

const char *reg_names[] = {[RBX] = "RBX", [RCX] = "RCX", [RDX] = "RDX", [RSI] = "RSI", [RDI] = "RDI", [RBP] = "RBP",
                           [RSP] = "RSP", [RAX] = "RAX", [R8] = "R8",   [R9] = "R9",   [R10] = "R10", [R11] = "R11",
                           [R12] = "R12", [R13] = "R13", [R14] = "R14", [R15] = "R15"};

typedef enum {
    ZF
} Flag_enum;

typedef struct {
    char name[LINE_BUFFER_SIZE];
    int address;
} Label;

Label label_arr[LABEL_BUF_SIZE];
int last_lable_inex = 0;

typedef struct {
    int64_t REGS[16];
    int flag[FLAG_COUNTER];
    int pc;
    int is_halted;

} CPU;

typedef enum {
    MEM_ADDR,
    VALUE,
    REG,
    LABEL

} Type_of_operand;

typedef struct {
    Type_of_operand type;
    int value;
    char label[LINE_BUFFER_SIZE];
} Operand;

typedef struct {
    Opcode opcode;
    Operand operand1;
    Operand operand2;
    Operand operand3;

} Instr;

int isnumber(char *word) {
    int i = 0;
    if (word[0] == '-')
        i++;
    for (; word[i] != '\0'; i++) {
        if (word[i] < '0' || word[i] > '9')
            return 0;
    }
    return 1;
}

int sizeof_dArr(char **a) {
    int length = 0;
    while (a[length] != NULL)
        length++;

    return length;
}
int is_line_label(char *line) {
    int i = 0;
    for (; line[i] != '\0' && line[i] != ':'; i++) {
    }
    if (line[i] == ':') {
        line[i] = '\0';
        return 1;
    }
    return 0;
}

char **split(char line[], char symbol) {
    int i = 0;
    int j = 0;
    char **words = (char **)malloc((LINE_BUFFER_SIZE) * (sizeof(char *)));

    int k = 0;
    while (line[i] == symbol)
        i++;
    while (line[i] != '\0' && line[i] != EOF && line[i] != '\n') {
        char *word = malloc(LINE_BUFFER_SIZE);
        for (; line[i] != '\n' && line[i] != symbol && line[i] != '\0' && line[i] != EOF; i++) {
            word[k] = line[i];
            k++;
        }
        word[k] = '\0';
        k = 0;
        words[j] = word;
        j++;

        if (line[i] == '\0' || line[i] == EOF || line[i] == '\n')
            break;
        i++;
        while (line[i] == symbol)
            i++;
    }
    words[j] = NULL;
    return words;
}

Operand word_to_operand(char *word) {
    Operand operand;
    
    int k = 0;

    for (; word[k] != '\0' && word[k] != ','; k++) {
    }
    word[k] = '\0';
    if (word[0] == '[') {
        word += 1;
        int i = 0;
        for (; word[i] != '\0' && word[i] != ']' && word[i] != ','; i++) {
        }
        word[i] = '\0';
        operand.type = MEM_ADDR;
        operand.value = strtol(word, NULL, 10);
    } else if (isnumber(word)) {
        operand.type = VALUE;
        operand.value = strtol(word, NULL, 10);
    } else {
        for (int i = 0; i < (sizeof(reg_names) / sizeof(reg_names[0])); i++) {
            if (strcmp(reg_names[i], word) == 0) {
                operand.type = REG;
                operand.value = i;
                return operand;
            }
        }
        operand.type = LABEL;
         strcpy(operand.label, word);
    }
    return operand;
}

Opcode word_to_opcode(char *word) {
    Opcode opcode;
    for (int i = 0; i < (sizeof(opcode_names) / sizeof(opcode_names[0])); i++) {
        if (strcmp(opcode_names[i], word) == 0) {
            opcode = i;
        }
    }
    return opcode;
}


Instr *asembler(char file_name[], int *number_of_instructions) {
    // Open file

    Instr *instr_list = malloc(sizeof(Instr) * INSTRUCTION_BUF);

    CPU cpu;

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Couldnt't open the file!");
        return NULL;
    }

    char buf[LINE_BUFFER_SIZE];
    int i = 0;

    while (fgets(buf, LINE_BUFFER_SIZE, fp) != NULL) {
        Instr instr;
        int line_s = 0;
        char **line = split(buf, ' ');
        int skip_line = 0;
        line_s = sizeof_dArr(line);
        skip_line = 0;
        switch (line_s) {
        case 0:
            skip_line = 1;
            break;
        case 1:
            if (is_line_label(line[0])) {
                strcpy(label_arr[last_lable_inex].name, line[0]);
                label_arr[last_lable_inex].address = i;
                skip_line = 1;
                last_lable_inex++;
            } else {
                instr.opcode = word_to_opcode(line[0]);
            }

            break;
        case 2:
            instr.opcode = word_to_opcode(line[0]);
            instr.operand1 = word_to_operand(line[1]);
            break;
        case 3:
            instr.opcode = word_to_opcode(line[0]);
            instr.operand1 = word_to_operand(line[1]);
            instr.operand2 = word_to_operand(line[2]);

            break;
        }
        if (!skip_line) {
            instr_list[i] = instr;
            i++;
        }
        
    }

    *number_of_instructions = i;

    fclose(fp);
    return instr_list;
}

void compiler(CPU *cpu, Instr *instructions, int number_of_instruction) {
    

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction) {
        int jumped = 0;
        instr = instructions + cpu->pc;
        if (instr->opcode == MOV) {
            if (instr->operand1.type == REG && instr->operand2.type == REG) {
                cpu->REGS[instr->operand1.value] = cpu->REGS[instr->operand2.value];
            } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
                cpu->REGS[instr->operand1.value] = instr->operand2.value;
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] = cpu->REGS[instr->operand2.value];
                }
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] = instr->operand2.value;
                }
            }
        } else if (instr->opcode == ADD) {
            if (instr->operand1.type == REG && instr->operand2.type == REG) {
                cpu->REGS[instr->operand1.value] += cpu->REGS[instr->operand2.value];
            } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
                cpu->REGS[instr->operand1.value] += instr->operand2.value;
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] += cpu->REGS[instr->operand2.value];
                }
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] += instr->operand2.value;
                }
            }
        } else if (instr->opcode == SUB) {
            if (instr->operand1.type == REG && instr->operand2.type == REG) {
                cpu->REGS[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
            } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
                cpu->REGS[instr->operand1.value] -= instr->operand2.value;
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
                }
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value] -= instr->operand2.value;
                }
            }
        } else if (instr->opcode == DEC) {
            if (instr->operand1.type == REG) {
                cpu->REGS[instr->operand1.value]--;
            } else if (instr->operand1.type == MEM_ADDR) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value]--;
                }
            }
        } else if (instr->opcode == INC) {
            if (instr->operand1.type == REG) {
                cpu->REGS[instr->operand1.value]++;
            } else if (instr->operand1.type == MEM_ADDR) {
                if (instr->operand1.value < MEMSIZE) {
                    MEMORY[instr->operand1.value]++;
                }
            }
        } else if (instr->opcode == CMP) {
            if (instr->operand1.type == REG && instr->operand2.type == REG) {
                if (cpu->REGS[instr->operand1.value] - cpu->REGS[instr->operand2.value] == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
                if (cpu->REGS[instr->operand1.value] - instr->operand2.value == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
                if (instr->operand1.value < MEMSIZE) {
                    if (MEMORY[instr->operand1.value] - cpu->REGS[instr->operand2.value] == 0)
                        cpu->flag[ZF] = 1;
                    else
                        cpu->flag[ZF] = 0;
                }
            } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
                if (instr->operand1.value < MEMSIZE) {
                    if (MEMORY[instr->operand1.value] - instr->operand2.value == 0)
                        cpu->flag[ZF] = 1;
                    else
                        cpu->flag[ZF] = 0;
                }
            }

        } else if (instr->opcode == JMP && cpu->flag[ZF]) {
            for(int i = 0; i < last_lable_inex;i++){
                if(strcmp(label_arr[i].name,instr->operand1.label)==0){
                    cpu->pc = label_arr[i].address;
                    jumped = 1;
                }   
            }

        }
    
        if(!jumped){
            cpu->pc++;
        
        }
        
    }
}
CPU *cpu_init(CPU *processor) {
    CPU *cpu = processor;

    memset(cpu, 0, sizeof(*cpu));

    return cpu;
}

int main(void) {
    int number_of_instruction;
    Instr *instr_list = asembler("program.asm", &number_of_instruction);
    CPU processor;
    CPU *cpu = cpu_init(&processor);
    compiler(cpu, instr_list, number_of_instruction);

    free(instr_list);

    return 0;
}
