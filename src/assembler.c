
#include "assembler.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[LINE_BUF_SIZE];
    int address;
} Label;

typedef struct {
    Instr *instr_arr;
    Label *label_arr;

    int last_label_index;
    int last_instr_index;

    int label_arr_max_size;
    int instr_arr_max_size;

    int exit_error;
} Assembler;

static int find_label(const char *name, Assembler *assembler) {
    for (int i = 0; i < assembler->last_label_index; i++) {
        if (strcmp(assembler->label_arr[i].name, name) == 0)
            return assembler->label_arr[i].address;
    }
    return -1;
}

static int equals_ignore_case(const char *a, const char *b) {
    int i = 0;
    for (; a[i] != '\0' && b[i] != 0; i++) {
        if(!(toupper((unsigned char)a[i]) == toupper((unsigned char)b[i]))) return 0;
    }
    if(a[i] == '\0' && b[i] == '\0') return 1; 
    return 0;
}
static int isnumber(char *word) {
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

static int sizeof_dArr(char **a) {
    int length = 0;
    while (a[length] != NULL)
        length++;

    return length;
}
static int is_line_label(char *line) {
    int i = 0;
    for (; line[i] != '\0' && line[i] != ':'; i++) {
    }
    if (line[i] == ':') {
        line[i] = '\0';
        return 1;
    }
    return 0;
}
static void line_free(char **line, int size) {
    for (int i = 0; i < size; i++) {
        free(line[i]);
    }
    free(line);
}
static int char_is_end(char a) {
    if (a == '\n' || a == '\r' || a == COMMENT_SYMBOL || a == '\0')
        return 1;
    return 0;
}
static int char_is_split(char a) {
    if (a == '\t' || a == ' ' || a == ',')
        return 1;
    return 0;
}
static char **split(char line[]) {
    int i = 0;
    int j = 0;
    char **words = (char **)malloc((LINE_BUF_SIZE) * (sizeof(char *)));

    int k = 0;
    while (char_is_split(line[i]))
        i++;

    while (!char_is_end(line[i])) {
        char *word = malloc(LINE_BUF_SIZE);
        for (; !char_is_end(line[i]) && !char_is_split(line[i]); i++) {
            word[k] = line[i];
            k++;
        }
        word[k] = '\0';
        k = 0;
        words[j] = word;
        j++;
        while (char_is_split(line[i]))
            i++;
    }
    words[j] = NULL;
    return words;
}

static Operand word_to_operand(char *word, int line_number, Assembler *assembler) {
    Operand operand = {0};

    int k = 0;

    for (; word[k] != '\0'; k++) {
    }
    if (word[0] == '[') {
        if (word[k - 1] != ']') {
            fprintf(stderr, "line %d: missing ']' in %s\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
        }
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

        for (int i = 0; i < REG_COUNT; i++) {
            if (equals_ignore_case(word, reg_names[i])) {
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

static int word_to_opcode(char *word) {
    for (int i = 0; i < OPCODE_COUNT; i++) {
        if (equals_ignore_case(word, opcode_names[i]))
            return i;
    }
    return -1;
}
Assembler *assembler_init(Assembler *assembler) {

    assembler->exit_error = 0;

    assembler->label_arr = (Label *)malloc(sizeof(Label) * LABEL_BUF_SIZE);
    assembler->last_label_index = 0;
    assembler->instr_arr = (Instr *)malloc(sizeof(Instr) * INSTRUCTION_BUF_SIZE);
    assembler->last_instr_index = 0;

    assembler->label_arr_max_size = LABEL_BUF_SIZE;
    assembler->instr_arr_max_size = INSTRUCTION_BUF_SIZE;
    return assembler;
}

Instr *assemble(const char file_name[], int *number_of_instructions) {

    Assembler assembler;
    assembler = *assembler_init(&assembler);

    FILE *fp = fopen(file_name, "r");

    if (!fp) {
        fprintf(stderr, "Couldn't open the file!\n");
        return NULL;
    }

    char buf[LINE_BUF_SIZE];
    int line_number = 1;
    int instruction_number = 0;

    while (fgets(buf, LINE_BUF_SIZE, fp) != NULL) {
        if (strchr(buf, '\n') == NULL && feof(fp) == 0) {
            fprintf(stderr, "line %d: %.15s... too long.\n", line_number, buf);
            return NULL;
        }
        if (assembler.last_label_index == assembler.label_arr_max_size) {
            Label *plabel = (Label *)realloc(assembler.label_arr, 2 * sizeof(Label) * assembler.last_label_index);
            if (plabel == NULL) {
                fprintf(stderr, "line %d: LABEL STACK OVERFLOW.\n", line_number);
                return NULL;
            }
            assembler.label_arr = plabel;
            assembler.label_arr_max_size *= 2;
        }
        if (assembler.last_instr_index == assembler.instr_arr_max_size) {
            Instr *pinstr = (Instr *)realloc(assembler.instr_arr, 2 * sizeof(Instr) * assembler.last_instr_index);
            if (pinstr == NULL) {
                fprintf(stderr, "line %d: INSTRUCTION STACK OVERFLOW.\n", line_number);
                return NULL;
            }
            assembler.instr_arr = pinstr;
            assembler.instr_arr_max_size *= 2;
        }

        Instr instr = {0};
        int line_s = 0;
        char **line = split(buf);
        int skip_line = 0;
        line_s = sizeof_dArr(line);
        char **no_label_line = line;
        int no_label_line_s = line_s;
        if (line_s == 0) {
            line_number++;
            continue;
        }
        if (is_line_label(line[0])) {
            strcpy(assembler.label_arr[assembler.last_label_index].name, line[0]);
            assembler.label_arr[assembler.last_label_index].address = instruction_number;
            assembler.last_label_index++;
            no_label_line++;
            no_label_line_s--;
        }
        if (no_label_line_s == 0) {
            line_number++;
            line_free(line, line_s);
            continue;
        } else {
            int opcode = word_to_opcode(no_label_line[0]);
            if (opcode == -1) {
                fprintf(stderr, "line %d: Syntax error: %s opcode was not found.\n", line_number, no_label_line[0]);
                assembler.exit_error = 1;
                return NULL;
            }
            instr.opcode = opcode;
        };
        switch (no_label_line_s) {

        case 3:
            instr.operand2 = word_to_operand(no_label_line[2], line_number, &assembler);
        /* fall through */
        case 2:
            instr.operand1 = word_to_operand(no_label_line[1], line_number, &assembler);
        /* fall through */
        case 1:
            break;

        default:
            fprintf(stderr, "line %d: More operands then expected. %s\n", line_number, buf);
            assembler.exit_error = 1;
            return NULL;
        }
        if (assembler.exit_error == 1) {
            line_free(line, line_s);
            return NULL;
        }
        if (!skip_line) {
            assembler.instr_arr[assembler.last_instr_index] = instr;
            assembler.last_instr_index++;
            instruction_number++;
        }
        line_number++;

        line_free(line, line_s);
    }

    for (int n = 0; n < instruction_number; n++) {
        Operand *op = &assembler.instr_arr[n].operand1;
        if (op->type != LABEL)
            continue;
        int address = find_label(op->label, &assembler);
        if (address < 0) {
            fprintf(stderr, "Unknown label %s.\n", op->label);
            return NULL;
        }
        op->value = address;
    }
    free(assembler.label_arr);

    *number_of_instructions = instruction_number;

    fclose(fp);
    return assembler.instr_arr;
}
