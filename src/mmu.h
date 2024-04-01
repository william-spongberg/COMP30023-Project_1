#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE 2048 //KB

typedef struct memory {
    int size;
    int available;
    // int8_t to reduce memory usage since we only store 1s and 0s
    int8_t *blocks;
} memory_t;

// Functions for creating block of memory and allocations
memory_t *create_memory();

/* 
    Memory management strategies, returns first available index 
    or -1 if no blocks available
*/
int8_t first_fit(memory_t *mem, int size);
int8_t paged_fit(memory_t *mem, int size);
int8_t virtual_fit(memory_t *mem, int size);

// Helper function
void free_memory(memory_t *mem, int start, int size);

// Clean up
void destroy_memory(memory_t *mem);

