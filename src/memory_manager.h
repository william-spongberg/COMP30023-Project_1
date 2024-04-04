#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "least_recently_used.h"
#include "read.h"
#include <math.h>

#define MEM_SIZE 2048 // KB
#define NUM_PAGES 512 // 2048 / 4

// Refactor page from least_recently_used.h, improve cohesion
typedef struct Page {
    page_t *p;
    struct Page *next;
} Page;

// infinite memory struct
typedef struct block_memory_t{
    int size;
    int available;
    int blocks[MEM_SIZE];
} block_memory_t;

typedef struct frame_t{
    bool is_allocated;
} frame_t;

// paged memory struct
typedef struct paged_memory_t{
    int size;
    int frames_available;
    frame_t frames[NUM_PAGES];
} paged_memory_t;

// method signatures
void *create_memory(mem_strategy strategy);
block_memory_t *create_block_memory();
paged_memory_t *create_paged_memory();

bool attempt_allocation(Process **p, void **mem, mem_strategy strategy, pqueue_t *lru_queue, int sim_time);
bool first_fit_allocation(Process **p, block_memory_t *mem);
bool paged_allocation(Process **p, paged_memory_t **mem, pqueue_t *lru_queue, int sim_time);
bool virtual_allocation(Process **p, paged_memory_t *mem, pqueue_t *lru_queue, int sim_time);
void create_pages(Process **p);

int16_t first_fit(block_memory_t *mem, int size);
void paged_fit(Process **p, paged_memory_t **mem);
int16_t virtual_fit(block_memory_t *mem, int size);

void free_memory(void **mem, Process **p, mem_strategy strategy, int start, int size);
void free_block_memory(block_memory_t *mem, int start, int size);
void free_paged_memory(paged_memory_t **mem, Process *p);

void destroy_memory(void *mem, mem_strategy strategy);
void destroy_block_memory(block_memory_t *mem);
void destroy_paged_memory(paged_memory_t *mem);

void print_memory_process(Process *p, block_memory_t *mem, int sim_time);
void print_paged_process(Process *p, paged_memory_t *mem, int sim_time);
void print_evicted_frames(Process *p, int sim_time);
int get_block_mem_usage(block_memory_t *mem);
int get_paged_mem_usage(paged_memory_t *mem);

#endif