#include "memory_manager.h"

// TODO: return void* for memory, unknown memory struct type
// create block of memory
void *create_memory(mem_strategy strategy) {
    switch (strategy) {
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
    mem->available = mem->size;
    for (int i = 0; i < mem->available; i++) {
        mem->blocks[i] = 0;
    }
    return mem;
}

// FIXME: Does this function create the physical memory or the page table for
// the process? Assume this is physical memory, only the frame number is needed
paged_memory_t *create_paged_memory() {
    paged_memory_t *mem = (paged_memory_t *)malloc(sizeof(paged_memory_t));
    mem->size = MEM_SIZE;
    mem->frames_available = NUM_PAGES;
    for (int i = 0; i < mem->frames_available; i++) {
        mem->frames[i].is_allocated = false;
    }
    mem->lru = NULL;
    return mem;
}

bool attempt_allocation(Process **p, void **mem, mem_strategy strategy) {
    if (mem == NULL)
        return true;
    switch (strategy) {
    case INFINITE:
        // don't need to allocate memory
        return true;
    case FIRST_FIT:
        return first_fit_allocation(p, *((block_memory_t **)mem));
    case PAGED:
        return paged_allocation(p, (paged_memory_t **)mem);
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

bool paged_allocation(Process **p, paged_memory_t **mem) {
    // FIXME: Is this check necessary? Since the process will only call this
    // function if it has not been allocated memory check whether p's pages are
    // allocated in memory if allocated, return true if not allocated if memory
    // available, allocate memory, return true
    // FIXME: Update page table in process struct and update paged_memory_t
    // struct
    // if memory not available
    // FIXME: according to task3 description, it should not evict the tails of
    // the queue but rather the least recently accessed pages
    // FIXME: the queue is not an input to this function, so eviction cannot be
    // done here suggestion: return false and handle eviction in the
    // process_manager.c evict tails of queue until enough free frames
    // FIXME: once the process manager evicts the least recently accessed pages,
    // it should call this function again to allocate memory allocate memory,
    // return true
    // return false (default)

    if ((*p)->pages == NULL) {
        create_pages(p);
    }

    // if pages are allocated, return true
    bool allocated = true;
    for (int i = 0; i < (*p)->mem / PAGE_SIZE; i++) {
        if (!((*p)->pages[i].is_allocated)) {
            allocated = false;
        }
    }
    if (allocated) {
        return true;
    }

    // if pages not allocated, allocate frames
    if ((*mem)->frames_available >= (*p)->mem / PAGE_SIZE) {
        paged_fit(p, mem);
        return true;
    } else {
        // evict least recently used pages until enough free frames
        Page *tail = NULL;
        while ((*mem)->frames_available < (*p)->mem / PAGE_SIZE) {
            tail = get_tail(&(*mem)->lru);
            (*mem)->frames[tail->p->frame_num].is_allocated = false;
            (*mem)->frames_available++;
            //delete_page(&(*mem)->lru, tail);
        }
        // now do paged fit
        paged_fit(p, mem);
        return true;
    }
    return false;
}

void create_pages(Process **p) {
    (*p)->pages = (page_t *)malloc(sizeof(page_t) * ((*p)->mem / PAGE_SIZE));
    for (int i = 0; i < (*p)->mem / PAGE_SIZE; i++) {
        (*p)->pages[i].frame_num = -1;
        (*p)->pages[i].is_allocated = false;
    }
}

// implement paged fit strategy
void paged_fit(Process **p, paged_memory_t **mem) {
    page_t *page = NULL;
    for (int i = 0; i < (*p)->mem / PAGE_SIZE; i++) {
        page = &(*p)->pages[i];
        insert_page(&(*mem)->lru, page);
    }

    int i = 0;
    int j = 0;
    while (i < (*mem)->size && j < (*p)->mem / PAGE_SIZE) {
        if (!(*mem)->frames[i].is_allocated) {
            (*p)->pages[j].frame_num = i;
            (*p)->pages[j].is_allocated = true;
            (*mem)->frames[i].is_allocated = true;
            (*mem)->frames_available--;
            j++;
        }
        i++;
    }
}

// free memory
void free_memory(void **mem, Process **p, mem_strategy strategy, int start,
                 int size) {
    if (mem != NULL) {
        switch (strategy) {
        case INFINITE:
            // no memory to free
            break;
        case FIRST_FIT:
            free_block_memory(*(block_memory_t **)mem, start, size);
            break;
        case PAGED:
            free_paged_memory((paged_memory_t **)mem, *p);
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

void free_paged_memory(paged_memory_t **mem, Process *p) {
    for (int i = 0; i < p->mem / PAGE_SIZE; i++) {
        (*mem)->frames[p->pages[i].frame_num].is_allocated = false;
        (*mem)->frames_available++;
        delete_page(&(*mem)->lru, get_page(&(*mem)->lru, &p->pages[i]));
    }
}

// clean up
void destroy_memory(void *mem, mem_strategy strategy) {
    if (mem != NULL) {
        switch (strategy) {
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
        sim_time, get_state(p), p->name, p->rtime, get_block_mem_usage(mem),
        p->addr);
}

// print paged process
void print_paged_process(Process *p, paged_memory_t *mem, int sim_time) {
    if (mem == NULL) {
        fprintf(stderr, "mem is NULL\n");
        exit(EXIT_FAILURE);
    }
    printf(
        "%d,%s,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[",
        sim_time, get_state(p), p->name, p->rtime, get_paged_mem_usage(mem));
    for (int i = 0; i < p->mem / PAGE_SIZE; i++) {
        if (i == p->mem / PAGE_SIZE - 1) {
            printf("%d]\n", p->pages[i].frame_num);
        } else {
            printf("%d,", p->pages[i].frame_num);
        }
    }
}

//void print_evicted_frames()

int get_block_mem_usage(block_memory_t *mem) {
    return ceil(100 - (((float)mem->available / (float)mem->size) * 100));
}

int get_paged_mem_usage(paged_memory_t *mem) {
    return ceil(
        100 -
        (((float)mem->frames_available * PAGE_SIZE / (float)mem->size) * 100));
}