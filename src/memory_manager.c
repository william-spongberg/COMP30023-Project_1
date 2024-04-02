#include "memory_manager.h"

// TODO: return void* for memory, unknown memory struct type
// create block of memory
void *create_memory(mem_strategy strategy) {
    switch(strategy) {
        case INFINITE:
            // no memory to create
            return NULL;
        case FIRST_FIT:
            // create block memory
            return create_block_memory();
        case PAGED:
            // create paged memory
            return create_paged_memory();
        default:
            fprintf(stderr, "Invalid memory strategy\n");
            exit(EXIT_FAILURE);
    }
    return NULL;
}

block_memory_t *create_block_memory() {
    block_memory_t *mem = (block_memory_t *)malloc(sizeof(block_memory_t));
    mem->size = MEM_SIZE;
    mem->available = MEM_SIZE;
    for (int i = 0; i < MEM_SIZE; i++) {
        mem->blocks[i] = 0;
    }
    return mem;
}

// FIXME: Does this function create the physical memory or the page table for the process? Assume this is physical memory, only the frame number is needed
paged_memory_t *create_paged_memory() {
    paged_memory_t *mem = (paged_memory_t *)malloc(sizeof(paged_memory_t));
    mem->size = MEM_SIZE;
    mem->available = MEM_SIZE;
    for (int i = 0; i < NUM_PAGES; i++) {
        mem->frames[i].is_allocated = false;
    }
    return mem;
}

bool attempt_allocation(Process **p, void *mem, mem_strategy strategy) {
    if (mem == NULL)
        return true;
    switch(strategy) {
        case INFINITE: 
            // don't need to allocate memory
            return true;
        case FIRST_FIT:
            return first_fit_allocation(p, (block_memory_t *)mem);
        case PAGED:
            // TODO: implement paged memory
            return true;
        default:
            fprintf(stderr, "Invalid memory strategy\n");
            exit(EXIT_FAILURE);
    }
    // default to true
    return true;
}

bool first_fit_allocation(Process **p, block_memory_t *mem) {
    if ((*p)->addr == -1) {
        // if memory available, allocate memory
        if (mem->available >= (*p)->mem) {
            (*p)->addr = first_fit(mem, (*p)->mem);
            return true;
        }
        // if memory not available, continue to next iteration
        else {
            return false;
        }
    }
    // memory already allocated
    else {
        return true;
    }
}

bool paged_allocation(Process *p, paged_memory_t *mem) {
    // check whether p's pages are allocated in memory
        // if allocated, return true
        // if not allocated
            // if memory available, allocate memory, return true
            // if memory not available
                // FIXME: according to task3 description, it should evict the tails of the queue but rather the least recently accessed pages
                // evict tails of queue until enough free frames
                // allocate memory, return true
    // return false (default)


}

// implement first fit strategy
int16_t first_fit(block_memory_t *mem, int size) {
    int start = -1;
    int count = 0;
    for (int i = 0; i < mem->size; i++) {
        if (mem->blocks[i] == 0) {
            if (start == -1) {
                start = i;
            }
            count++;
        } else {
            start = -1;
            count = 0;
        }
        if (count == size) {
            for (int j = start; j < start + size; j++) {
                mem->blocks[j] = 1;
            }
            mem->available -= size;
            return start;
        }
    }
    return -1;
}

// free memory
void free_memory(void *mem, mem_strategy strategy, int start, int size) {
    if (mem != NULL) {
        switch(strategy) {
            case INFINITE:
                // no memory to free
                break;
            case FIRST_FIT:
                free_block_memory((block_memory_t *)mem, start, size);
                break;
            case PAGED:
                // TODO: implement paged memory
                break;
            default:
                fprintf(stderr, "Invalid memory strategy\n");
                exit(EXIT_FAILURE);
        }
    }
}

void free_block_memory(block_memory_t *mem, int start, int size) {
    for (int i = start; i < start + size; i++) {
        mem->blocks[i] = 0;
    }
    mem->available += size;
}

// clean up
void destroy_memory(void *mem, mem_strategy strategy) {
    if (mem != NULL) {
        switch(strategy) {
            case INFINITE:
                // no memory to destroy
                break;
            case FIRST_FIT:
                destroy_block_memory((block_memory_t *)mem);
                break;
            case PAGED:
                destroy_paged_memory((paged_memory_t *)mem);
                break;
            default:
                fprintf(stderr, "Invalid memory strategy\n");
                exit(EXIT_FAILURE);
        }
    }
}

void destroy_block_memory(block_memory_t *mem) {
    free(mem);
}

void destroy_paged_memory(paged_memory_t *mem) {
    // TODO: implement paged memory
    free(mem);
}

// print memory usage
void print_memory_process(Process *p, block_memory_t *mem, int sim_time) {
    if (mem == NULL) {
        fprintf(stderr, "mem is NULL\n");
        exit(EXIT_FAILURE);
    }
    printf(
        "%d,%s,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%"
        "d\n",
        sim_time, get_state(p), p->name, p->rtime, get_mem_usage(mem), p->addr);
}

int get_mem_usage(block_memory_t *mem) {
    return ceil(100 - (((float)mem->available / (float)MEM_SIZE) * 100));
}