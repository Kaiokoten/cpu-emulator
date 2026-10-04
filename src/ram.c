#include "ram.h"

int64_t MEMORY[MEMSIZE] = {0};

int is_valid_mem_addr(int64_t addr) {
    return addr >= 0 && addr < MEMSIZE;
}