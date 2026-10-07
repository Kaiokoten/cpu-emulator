
#include "assembler.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"

typedef struct {
    char name[LABEL_NAME_SIZE];
    int address;
} Label;

typedef struct {
    Instr *instr_arr;
    int *instr_lines;
    Label *label_arr;

    int last_label_index;
    int last_instr_index;

    int label_arr_max_size;
    int instr_arr_max_size;

    

    int exit_error;

} Assembler;

enum { ALLOW_REG = 1, ALLOW_MEM = 2, ALLOW_VAL = 4, ALLOW_LABEL = 8 };

typedef struct {
    int number_of_operands;
    int arg_types[2];

} InstructionRule;

static const InstructionRule instruction_allow[] = {
    [MOV] = {2, {ALLOW_REG | ALLOW_MEM, ALLOW_REG | ALLOW_MEM | ALLOW_VAL}},
    [ADD] = {2, {ALLOW_REG | ALLOW_MEM, ALLOW_REG | ALLOW_MEM | ALLOW_VAL}},
    [SUB] = {2, {ALLOW_REG | ALLOW_MEM, ALLOW_REG | ALLOW_MEM | ALLOW_VAL}},

    [DEC] = {1, {ALLOW_REG | ALLOW_MEM}},
    [INC] = {1, {ALLOW_REG | ALLOW_MEM}},

    [CMP] = {2, {ALLOW_MEM | ALLOW_REG | ALLOW_VAL, ALLOW_MEM | ALLOW_REG | ALLOW_VAL}},
    [JMP] = {1, {ALLOW_LABEL}},
    [JE] = {1, {ALLOW_LABEL}},
    [HALT] = {0, {0}},
    [PRINT] = {1, {ALLOW_MEM | ALLOW_REG | ALLOW_VAL}}

};

static int find_label(const char *name, const Assembler *assembler) {
    for (int i = 0; i < assembler->last_label_index; i++) {
        if (strcmp(assembler->label_arr[i].name, name) == 0)
            return assembler->label_arr[i].address;
    }
    return -1;
}

static int equals_ignore_case(const char *a, const char *b) {
    int i = 0;
    for (; a[i] != '\0' && b[i] != 0; i++) {
        if (!(toupper((unsigned char)a[i]) == toupper((unsigned char)b[i])))
            return 0;
    }
    if (a[i] == '\0' && b[i] == '\0')
        return 1;
    return 0;
}

static int sizeof_dArr(char **a) {
    int length = 0;
    while (a[length] != NULL)
        length++;

    return length;
}
static int is_line_label(char *word) {
    size_t len = strlen(word);
    if (len > 0 && word[len - 1] == ':') {
        word[len - 1] = '\0';
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

    errno = 0;
    char *end;

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
        for (; word[i] != '\0' && word[i] != ']'; i++) {
        }
        word[i] = '\0';
        operand.type = MEM_ADDR;
        operand.value = strtoll(word, &end, 10);
        if (errno == ERANGE || operand.value < 0 || operand.value >= MEMSIZE) {
            fprintf(stderr, "line %d: memory address out of range '[%s]'.\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
        }
        if (end == word || *end != '\0') {
            fprintf(stderr, "line %d: invalid memory address '[%s]'.\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
        }
    } else if (word[0] == '+' || word[0] == '-' || isdigit((unsigned char)word[0])) {
        operand.type = VALUE;
        operand.value = strtoll(word, &end, 10);
        if (errno == ERANGE) {
            fprintf(stderr, "line %d: number out of range '%s'.\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
        }
        if (*end != '\0') {
            fprintf(stderr, "line %d: invalid number '%s'.\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
        }
    } else {
        for (int i = 0; i < REG_COUNT; i++) {
            if (equals_ignore_case(word, reg_names[i])) {
                operand.type = REG;
                operand.value = i;
                return operand;
            }
        }
        if (k >= LABEL_NAME_SIZE) {
            fprintf(stderr, "line %d: label '%s' is too long.\n", line_number, word);
            assembler->exit_error = 1;
            return operand;
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
static int operand_bit(Type_of_operand type) {
    switch (type) {
    case REG:
        return ALLOW_REG;
    case MEM_ADDR:
        return ALLOW_MEM;
    case VALUE:
        return ALLOW_VAL;
    case LABEL:
        return ALLOW_LABEL;
    default:
        break;
    }
    return 0;
}

static int validate_label(const Assembler *assembler, const char *name, int line_number) {
    int i = 0;
    if (name[0] == '\0') {
        fprintf(stderr, "line %d: label '%s' empty.\n", line_number, name);
        return 0;
    }
    if (isalpha((unsigned char) name[0]) || name[0] == '_') {
        i++;
        for (; name[i] != '\0'; i++) {
            if (!(isalpha((unsigned char) name[i]) || name[i] == '_' || isdigit((unsigned char) name[i]))) {
                fprintf(stderr, "line %d: label '%s' contains an invalid symbol.\n", line_number, name);
                return 0;
            }
        }
    } else {
        fprintf(stderr, "line %d: label '%s' starts with an invalid symbol.\n", line_number, name);
        return 0;
    }
    if (i >= LABEL_NAME_SIZE) {
        fprintf(stderr, "line %d: label '%s' is too long.\n", line_number, name);
        return 0;
    }
    for (int k = 0; k < REG_COUNT; k++) {
        if (equals_ignore_case(name, reg_names[k])) {
            fprintf(stderr, "line %d: label '%s' is a register name.\n", line_number, name);
            return 0;
        }
    }
    for (int k = 0; k < OPCODE_COUNT; k++) {
        if (equals_ignore_case(name, opcode_names[k])) {
            fprintf(stderr, "line %d: label '%s' is a opcode name.\n", line_number, name);
            return 0;
        }
    }
    if (find_label(name, assembler) != -1) {
        fprintf(stderr, "line %d: label '%s' mentioned multiple times.\n", line_number, name);
        return 0;
    }

    return 1;
}

static int validate_instruction(const Instr *instr, int operand_count, int line_number) {
    if (operand_count != instruction_allow[instr->opcode].number_of_operands) {
        fprintf(stderr, "line %d: '%s' requires %d operands, got %d.\n", line_number, opcode_names[instr->opcode],
                instruction_allow[instr->opcode].number_of_operands, operand_count);
        return 0;
    }
    const Operand ops[2] = {instr->operand1, instr->operand2};
    const InstructionRule *rule = &instruction_allow[instr->opcode];

    for (int i = 0; i < rule->number_of_operands; i++) {
        if ((operand_bit(ops[i].type) & rule->arg_types[i]) == 0) {
            fprintf(stderr, "line %d: '%s' operand %d has a wrong type.\n", line_number, opcode_names[instr->opcode],
                    i + 1);
            return 0;
        }
    }
    return 1;
}
Assembler *assembler_init(Assembler *assembler) {

    assembler->exit_error = 0;

    assembler->label_arr = (Label *)malloc(sizeof(Label) * LABEL_BUF_SIZE);
    assembler->last_label_index = 0;
    assembler->instr_arr = (Instr *)malloc(sizeof(Instr) * INSTRUCTION_BUF_SIZE);
    assembler->last_instr_index = 0;

    assembler->label_arr_max_size = LABEL_BUF_SIZE;
    assembler->instr_arr_max_size = INSTRUCTION_BUF_SIZE;

    assembler->instr_lines = malloc(sizeof(int) * INSTRUCTION_BUF_SIZE);

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
        if (assembler.last_instr_index == assembler.instr_arr_max_size) {
            int new_size = assembler.instr_arr_max_size * 2;

            Instr *pinstr = realloc(assembler.instr_arr, sizeof(*pinstr) * new_size);
            if (pinstr == NULL) {
                fprintf(stderr, "line %d: out of memory.\n", line_number);
                return NULL;
            }
            assembler.instr_arr = pinstr;

            int *plines = realloc(assembler.instr_lines, sizeof(*plines) * new_size);
            if (plines == NULL) {
                fprintf(stderr, "line %d: out of memory.\n", line_number);
                return NULL;
            }
            assembler.instr_lines = plines;

            assembler.instr_arr_max_size = new_size;
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
            if (validate_label(&assembler, line[0], line_number) == 0) {
                assembler.exit_error = 1;
                free(assembler.label_arr);
                return NULL;
            }
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
        }
        if (assembler.exit_error == 1) {
            line_free(line, line_s);
            return NULL;
        }
        if (!skip_line) {
            if (validate_instruction(&instr, no_label_line_s - 1, line_number) == 0) {
                assembler.exit_error = 1;
                line_free(line, line_s);
                return NULL;
            }
            assembler.instr_arr[assembler.last_instr_index] = instr;
            assembler.instr_lines[assembler.last_instr_index] = line_number;
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
            fprintf(stderr, "line %d: Syntax error: %s label was not found.\n", assembler.instr_lines[n], op->label);
            return NULL;
        }
        op->value = address;
    }

    free(assembler.label_arr);
    free(assembler.instr_lines);

    *number_of_instructions = instruction_number;

    fclose(fp);
    return assembler.instr_arr;
}
