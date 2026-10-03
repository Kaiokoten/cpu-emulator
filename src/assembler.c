
#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char name[LINE_BUFFER_SIZE];
    int address;
} Label;

static Label label_arr[LABEL_BUF_SIZE];
static int last_lable_index = 0;


static int find_label(const char *name) {
    for (int i = 0; i < last_lable_index; i++) {
        if (strcmp(label_arr[i].name, name) == 0)
            return label_arr[i].address;
    }
    return -1;
}

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

Instr *asembler(const char file_name[], int *number_of_instructions) {
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
        for (int k = 0; k < line_s; k++) {
            free(line[k]);
        }
        free(line);
    }
    for (int n = 0; n < i; n++) {
        Operand *op = &instr_list[n].operand1;
        if (op->type != LABEL)
            continue;
        int address = find_label(op->label);
        if (address < 0) {
            printf("Unknown label %s\n", op->label);
            return NULL;
        }
        op->value = address;
    }
    *number_of_instructions = i;

    fclose(fp);
    return instr_list;
}
