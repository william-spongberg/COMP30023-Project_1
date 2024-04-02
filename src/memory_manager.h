#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include "process.h"
#include <math.h>

#define MEM_SIZE 2048 // KB

typedef struct memory {
    int size;
    int available;
    // MEM_SIZE number of possible memory spots
    int blocks[MEM_SIZE];
    // char *strategy;
} memory_t;

// method signatures
memory_t *create_memory(char *memory_strategy);

int16_t allocate_memory(memory_t *mem, char *memory_strategy, int size);
int16_t first_fit(memory_t *mem, int size);
int16_t paged_fit(memory_t *mem, int size);
int16_t virtual_fit(memory_t *mem, int size);

void free_memory(memory_t *mem, int start, int size);
void destroy_memory(memory_t *mem);

int get_mem_usage(memory_t *mem);
void print_memory_process(Process *p, memory_t *mem, int sim_time);

#endif