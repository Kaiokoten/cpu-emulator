#include "ram.h"
#include <string.h>

void memory_init(Memory *mem) {
    memset(mem, 0, sizeof(*mem));
}

int is_valid_mem_addr(int64_t addr) {
    return addr >= 0 && addr < MEMSIZE;
}