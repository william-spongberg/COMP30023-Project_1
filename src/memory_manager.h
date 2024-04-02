#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include "queue.h"
#include "read.h"
#include <math.h>

#define MEM_SIZE 2048 // KB
#define PAGE_SIZE 4   // KB
#define NUM_PAGES 512 // 2048 / 4

// infinite memory struct
typedef struct block_memory_t{
    int size;
    int available;
    int blocks[MEM_SIZE];
} block_memory_t;

// paged memory struct
typedef struct page_t{
    int frame_num;
    char* process_id;
    bool is_allocated;
} page_t;

typedef struct frame_t{
    bool is_allocated;
} frame_t;

typedef struct paged_memory_t{
    int size;
    int available;
    page_t pages[NUM_PAGES];
    frame_t frames[NUM_PAGES];
} paged_memory_t;

// method signatures
void *create_memory(mem_strategy strategy);
block_memory_t *create_block_memory();
paged_memory_t *create_paged_memory();

bool attempt_allocation(Process **p, void *mem, mem_strategy strategy);
bool first_fit_allocation(Process **p, block_memory_t *mem);

int16_t first_fit(block_memory_t *mem, int size);
int16_t paged_fit(paged_memory_t *mem, int size);
int16_t virtual_fit(block_memory_t *mem, int size);

void free_memory(void *mem, mem_strategy strategy, int start, int size);
void free_block_memory(block_memory_t *mem, int start, int size);

void destroy_memory(void *mem, mem_strategy strategy);
void destroy_block_memory(block_memory_t *mem);
void destroy_paged_memory(paged_memory_t *mem);

void print_memory_process(Process *p, block_memory_t *mem, int sim_time);
int get_mem_usage(block_memory_t *mem);

#endif