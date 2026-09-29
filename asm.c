#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>



#define LINE_BUFFER_SIZE 50
#define MEMSIZE 4092


uint64_t MEMORY[MEMSIZE];


typedef enum{
    MOV
}Opcode;

const char *opcode_names[] = {
    [MOV] = "MOV",
};

typedef enum{
    RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15
}Reg;

const char *reg_names[] = {
    [RBX] = "RBX", [RCX] = "RCX", [RDX] = "RDX", [RSI] = "RSI",
    [RDI] = "RDI", [RBP] = "RBP", [RSP] = "RSP", [RAX] = "RAX",
    [R8] = "R8", [R9] = "R9", [R10] = "R10", [R11] = "R11",
    [R12] = "R12", [R13] = "R13", [R14] = "R14", [R15] = "R15"
};

typedef enum{
    RIP,
    RFLAGS
}Spesial_Reg;

typedef struct{
    int64_t REGS[16];

}CPU;

typedef enum{
    MEM_ADDR,
    VALUE,
    REG

}Type_of_operand;

typedef struct{

    Type_of_operand type;
    int value;
}Operand;


typedef struct{
    Opcode opcode;
    Operand operand1;
    Operand operand2;
    Operand operand3;

}Instr;

int isnumber(char *word){
    int i = 0;
    if(word[0] == '-') i++;
    for(; word[i] != '\0';i++){
        if(word[i] < '0' || word[i] > '9') return 0;
    }
    return 1;
}

int sizeof_dArr(char** a){
    int length = 0;
    while(a[length] != NULL) length++;

    return length;
}




char** split(char line[], char symbol){
    int i = 0;
    int j = 0;
    char** words = (char**)malloc((LINE_BUFFER_SIZE) * (sizeof(char*)));

    int k = 0;
    while(line[i] == symbol)i++;
    while(line[i] != '\0' && line[i] != EOF && line[i] !='\n'){
        char* word = malloc(LINE_BUFFER_SIZE);
        for(;line[i] != '\n' && line[i]!= symbol && line[i] != '\0'  && line[i] != EOF; i++){
            word[k] = line[i];
            k++;
        }
        word[k] = '\0';
        k = 0;
        words[j] = word;
        j++;

        if(line[i] == '\0'  || line[i] == EOF || line[i] =='\n') break;
        i++;
        while(line[i] == symbol)i++;
    }
    words[j] = NULL;
    return words;
}

Operand word_to_operand(char *word){
    Operand operand;
    int k = 0;
    for(; word[k] != '\0'  && word[k] != ',';k++){
            
        }
    word[k] = '\0';
    if(word[0] == '['){
        word += 1;
        int i = 0;
        for(; word[i] != '\0' && word[i] != ']' && word[i] != ',';i++){
            
        }
        word[i] = '\0';
        operand.type = MEM_ADDR;
        operand.value = strtol(word,NULL,10);

    }else if(isnumber(word)){
        
        operand.type = VALUE;
        operand.value = strtol(word,NULL,10);
        
        
    }else{
        for(int i = 0; i < (sizeof(reg_names) / sizeof(reg_names[0])); i++){
            if(strcmp(reg_names[i],word) == 0){
                operand.type = REG;
                operand.value = i;
            }
        }
    }
    return operand;

}

Opcode word_to_opcode(char *word){
    Opcode opcode;
    for(int i = 0;i < (sizeof(opcode_names) / sizeof(opcode_names[0]));i++){
        if(strcmp(opcode_names[i],word) == 0){
            opcode = i;
        }
    }
    return opcode;
}

Instr make_instr(char *buf){
    Instr instruction;
    int line_s = 0;
    char** line = split(buf,' ');
    
    line_s = sizeof_dArr(line);
    
    Instr instr;
    
    switch(line_s){
        case 0:
            printf("empty line");
            break;
        case 1:
            instr.opcode = word_to_opcode(line[0]);
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

    return instr;

}

void compiler(char file_name[]){
//Open file
    CPU cpu;

    FILE *fp = fopen(file_name,"r");
    if(fp == NULL){
        printf("Couldnt't open the file!");
        return;
    } 

    char buf[LINE_BUFFER_SIZE];


    while(fgets(buf,LINE_BUFFER_SIZE,fp)!=NULL){

        Instr instr;
        instr = make_instr(buf);

        if(instr.opcode == MOV){
            if(instr.operand1.type == REG && instr.operand2.type == REG ){
                cpu.REGS[instr.operand1.value] = cpu.REGS[instr.operand2.value];
            }else if(instr.operand1.type == REG && instr.operand2.type == VALUE ){
                  cpu.REGS[instr.operand1.value] = instr.operand2.value;

            }else if(instr.operand1.type == MEM_ADDR && instr.operand2.type == REG ){
                if(instr.operand1.value < MEMSIZE){
                    MEMORY[instr.operand1.value] = cpu.REGS[instr.operand2.value];
                    
                }
            }else if(instr.operand1.type == MEM_ADDR && instr.operand2.type == VALUE ){
                if(instr.operand1.value  < MEMSIZE){
                    MEMORY[instr.operand1.value] = instr.operand2.value;
                
                }
            }
            
                
        }


    }

        

        
        

        






    fclose(fp);

}
int main(void){



    compiler("program.asm");

    return 0;
}
