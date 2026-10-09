#ifndef RAM_H
#define RAM_H

#include <stdint.h>

#define MEMSIZE 4096

typedef struct{
    int64_t cells[MEMSIZE];
}Memory;

void memory_init(Memory *mem);
int is_valid_mem_addr(int64_t addr);

#endif