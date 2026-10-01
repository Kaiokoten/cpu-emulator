#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 50
#define MEMSIZE 4092
#define INSTRUCTION_BUF 100
#define FLAG_COUNTER 1
#define LABEL_BUF_SIZE 50

uint64_t MEMORY[MEMSIZE] = {0};

typedef enum {
    MOV,
    ADD,
    DEC,
    INC,
    SUB,
    CMP,
    JMP,
    JE,
    HALT,
    PRINT
} Opcode;

const char *opcode_names[] = {[MOV] = "mov",   [ADD] = "add",    [DEC] = "dec", [INC] = "inc",
                              [SUB] = "sub",   [CMP] = "cmp",    [JMP] = "jmp", [JE] = "je",
                              [HALT] = "halt", [PRINT] = "print"

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

const char *flag_names[] = {[ZF] = "ZF"};
typedef struct {
    char name[LINE_BUFFER_SIZE];
    int address;
} Label;

Label label_arr[LABEL_BUF_SIZE];
int last_lable_index = 0;

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
    int64_t value;
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

    if (word[0] == '-') {
        i++;
        if (word[i] == '\0') {
            return 0;
        }
    } else if (word[0] == '\0')
        return 0;

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
    Opcode opcode = -1;
    for (int i = 0; i < (sizeof(opcode_names) / sizeof(opcode_names[0])); i++) {
        if (strcmp(opcode_names[i], word) == 0) {
            opcode = i;
        }
    }
    return opcode;
}
void print_regs(CPU *cpu) {
    int quater = (sizeof(reg_names) / sizeof(reg_names[0])) >> 2;
    for (int i = 0; i < quater; i++) {
        for (int j = 0; j < quater; j++) {
            printf("[%s] = %lld\t", reg_names[quater * j + i], cpu->REGS[quater * j + i]);
        }
        printf("\n");
    }
}
void print_flags(CPU *cpu){
    int flags_s = sizeof(cpu->flag) / sizeof(cpu->flag[0]);
    for(int i = 0; i < flags_s; i++){
        printf("[%s] = %d\t",flag_names[i], cpu->flag[i]);


    }
    printf("\n");
}
int is_valid_mem_addr(int64_t value){
    return ((value >= 0 && value < MEMSIZE) ? 1 : 0);

}

void print_memory(void){
    int count_line = 0;
    char bit_number[sizeof(MEMORY[0]) *8+ 1];
    bit_number[sizeof(MEMORY[0]) * 8] = '\0';
    for(int i = 0; i < MEMSIZE; i++){
        if(MEMORY[i] != 0){
            for(int j = 0; j < sizeof(MEMORY[0])*8;j++){
                bit_number[sizeof(MEMORY[0]) * 8 - 1 - j] = '0' + ((MEMORY[i] >> j) & 1);
            }
            printf("MEMORY[%d] = %s", i, bit_number);
        }
    }


}
Instr *asembler(char file_name[], int *number_of_instructions) {
    // Open file

    Instr *instr_list = malloc(sizeof(Instr) * INSTRUCTION_BUF);

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Couldnt't open the file!");
        return NULL;
    }

    char buf[LINE_BUFFER_SIZE];
    int i = 0;

    while (fgets(buf, LINE_BUFFER_SIZE, fp) != NULL) {
        if (i == INSTRUCTION_BUF) {
            printf("COMMAND OVERFLOW!");
            return NULL;
        }

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
                if (last_lable_index < LABEL_BUF_SIZE) {
                    strcpy(label_arr[last_lable_index].name, line[0]);
                    label_arr[last_lable_index].address = i;
                    skip_line = 1;
                    last_lable_index++;
                } else {
                    printf("LABEL_BUF_SIZE OVERFLOW!");
                    return NULL;
                }
            } else {
                instr.opcode = word_to_opcode(line[0]);
                if (word_to_opcode(line[0]) == -1) {
                    printf("Syntacsis error %s", buf);
                    return NULL;
                };
            }

            break;
        case 2:
            instr.opcode = word_to_opcode(line[0]);
            if (word_to_opcode(line[0]) == -1) {
                printf("Syntacsis error %s", buf);
                return NULL;
            };
            instr.operand1 = word_to_operand(line[1]);
            break;
        case 3:
            instr.opcode = word_to_opcode(line[0]);
            if (word_to_opcode(line[0]) == -1) {
                printf("Syntacsis error %s", buf);
                return NULL;
            };
            instr.operand1 = word_to_operand(line[1]);
            instr.operand2 = word_to_operand(line[2]);

            break;
        default:
            continue;
        }

        if (!skip_line) {
            instr_list[i] = instr;
            i++;
        }
        for(int k = 0; k < line_s; k++){
            free(line[k]);
        }
        free(line);
    }

    *number_of_instructions = i;

    fclose(fp);
    return instr_list;
}

void execute_one(CPU *cpu, Instr *instr) {

    int jumped = 0;
    if (instr->opcode == MOV) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] = cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] = instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] = cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] = instr->operand2.value;
            }
        }
    } else if (instr->opcode == ADD) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] += cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] += instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] += cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] += instr->operand2.value;
            }
        }
    } else if (instr->opcode == SUB) {
        if (instr->operand1.type == REG && instr->operand2.type == REG) {
            cpu->REGS[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
        } else if (instr->operand1.type == REG && instr->operand2.type == VALUE) {
            cpu->REGS[instr->operand1.value] -= instr->operand2.value;
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == REG) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] -= cpu->REGS[instr->operand2.value];
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value] -= instr->operand2.value;
            }
        }
    } else if (instr->opcode == DEC) {
        if (instr->operand1.type == REG) {
            cpu->REGS[instr->operand1.value]--;
        } else if (instr->operand1.type == MEM_ADDR) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                MEMORY[instr->operand1.value]--;
            }
        }
    } else if (instr->opcode == INC) {
        if (instr->operand1.type == REG) {
            cpu->REGS[instr->operand1.value]++;
        } else if (instr->operand1.type == MEM_ADDR) {
            if (is_valid_mem_addr(instr->operand1.value)) {
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
            if (is_valid_mem_addr(instr->operand1.value)) {
                if (MEMORY[instr->operand1.value] - cpu->REGS[instr->operand2.value] == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            }
        } else if (instr->operand1.type == MEM_ADDR && instr->operand2.type == VALUE) {
            if (is_valid_mem_addr(instr->operand1.value)) {
                if (MEMORY[instr->operand1.value] - instr->operand2.value == 0)
                    cpu->flag[ZF] = 1;
                else
                    cpu->flag[ZF] = 0;
            }
        }

    } else if (instr->opcode == JE && cpu->flag[ZF]) {
        int found = 0;
        for (int i = 0; i < last_lable_index; i++) {
            if (strcmp(label_arr[i].name, instr->operand1.label) == 0) {
                cpu->pc = label_arr[i].address;
                jumped = 1;
                found = 1;
            }
        }
        if (!found) {
            printf("WRONG LABEL");
            cpu->is_halted = 1;
        }

    } else if (instr->opcode == JMP) {
        int found = 0;
        for (int i = 0; i < last_lable_index; i++) {
            if (strcmp(label_arr[i].name, instr->operand1.label) == 0) {
                cpu->pc = label_arr[i].address;
                jumped = 1;
                found = 1;
            }
        }
        if (!found) {
            printf("WRONG LABEL");
            cpu->is_halted = 1;
        }
        
    } else if (instr->opcode == PRINT) {
        if (instr->operand1.type == REG) {
            printf("%lld\n", cpu->REGS[instr->operand1.value]);
        } else if (instr->operand1.type == MEM_ADDR && is_valid_mem_addr(instr->operand1.value)) {
            printf("%llu\n", MEMORY[instr->operand1.value]);
        } else if (instr->operand1.type == VALUE) {
            printf("%lld\n", instr->operand1.value);
        }

    } else if (instr->opcode == HALT) {
        cpu->is_halted = 1;
    }

    if (!jumped) {
        cpu->pc++;
    }
}

void compiler(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction && !cpu->is_halted) {
        instr = instructions + cpu->pc;
        execute_one(cpu, instr);
    }
}

void debuger(CPU *cpu, Instr *instructions, int number_of_instruction) {

    Instr *instr = instructions;

    while (cpu->pc < number_of_instruction && !cpu->is_halted) {
        instr = instructions + cpu->pc;
        int executed_pc = cpu->pc;
        execute_one(cpu, instr);
        int choise = -1;
        printf("What you want to see:\n 1 - REG info\n2 - FLAGS info\n3 - MEMORY\n4 - pc\n");
        while ((choise = getchar()) != '\n' && choise != EOF) {
            if (choise == '0') {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {
                }
                break;
            }else if (choise == '1'){
                print_regs(cpu);
            }else if (choise == '2'){
                print_flags(cpu);
            }else if(choise == '3'){
                print_memory();
            }else if(choise == '4'){
                printf("%d - command [%s]",executed_pc, opcode_names[instr->opcode]);
            }
            
            
            printf("\n");
            printf("What you want to see:\n 1 - REG info\n2 - FLAGS info\n3 - MEMORY\n4 - pc\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
        }
    }
}

CPU *cpu_init(CPU *processor) {
    CPU *cpu = processor;

    memset(cpu, 0, sizeof(*cpu));

    return cpu;
}

int main(int argc, char *argv[]) {
    int file_given = 0;
    char name_of_file[LINE_BUFFER_SIZE] = {0};
    if (argc > 2) {
        if(!strcmp(argv[1],"-f") && (strlen(argv[2]) < LINE_BUFFER_SIZE)){
            strcpy(name_of_file,argv[2]);
            file_given = 1;
        }
    } 

    int number_of_instruction;
    
    Instr *instr_list = asembler(file_given ? name_of_file : "program.asm", &number_of_instruction);
    if (instr_list == NULL)
        return 0;
    CPU processor;
    CPU *cpu = cpu_init(&processor);
    
    debuger(cpu, instr_list, number_of_instruction);

    free(instr_list);

    return 0;
}
