#ifndef RAM_H
#define RAM_H

#include <stdint.h>

#define MEMSIZE 4096

extern int64_t MEMORY[MEMSIZE];

int is_valid_mem_addr(int64_t addr);

#endif