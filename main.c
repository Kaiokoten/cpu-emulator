#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>





// MAIN STACK-----------------------------------------------------------
typedef struct StackFrame{
    uint64_t data;
    struct StackFrame *next; 
} StackFrame;

struct StackFrame* push(uint64_t obj,StackFrame *top){

    struct StackFrame* new_top = malloc(sizeof(StackFrame));
    new_top->data = obj;
    new_top->next = top;

    return new_top;
}

StackFrame* pop(StackFrame *top){

    if(top->next == NULL) return top;
    StackFrame* l_top;
    l_top = top->next;
    free(top);
    
    return l_top;
}

void showStack(StackFrame *top){
    StackFrame *new_top = top;
    while(new_top->next != NULL){
        printf("%zu",new_top->data);
        new_top = new_top->next;
    }
}
//-------------------------------------------------------------------------

int main(int argc,char* argv[]){

    StackFrame* top = malloc(sizeof(StackFrame));
    top->next = NULL;
    

   




    free(top);
    return 0;
}