#include "memory_manager.h"

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
    case VIRTUAL:
        return create_paged_memory(); // Virtual strategy extends paged strategy
    default:
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

block_memory_t *create_block_memory() {
    block_memory_t *mem = (block_memory_t *)malloc(sizeof(block_memory_t));
    assert(mem != NULL);
    mem->size = MEM_SIZE;
    mem->available = mem->size;
    for (int i = 0; i < mem->available; i++) {
        mem->blocks[i] = 0;
    }
    return mem;
}

paged_memory_t *create_paged_memory() {
    paged_memory_t *mem = (paged_memory_t *)malloc(sizeof(paged_memory_t));
    assert(mem != NULL);
    mem->size = MEM_SIZE;
    mem->frames_available = NUM_PAGES;
    for (int i = 0; i < mem->frames_available; i++) {
        mem->frames[i].is_allocated = false;
    }
    return mem;
}

bool attempt_allocation(Process **p, void **mem, mem_strategy strategy,
                        pqueue_t *lru_queue, int sim_time) {
    if (mem == NULL)
        return true;
    switch (strategy) {
    case INFINITE:
        // don't need to allocate memory
        return true;
    case FIRST_FIT:
        return first_fit_allocation(p, *((block_memory_t **)mem));
    case PAGED:
        return paged_allocation(p, (paged_memory_t **)mem, lru_queue, sim_time);
    case VIRTUAL:
        return virtual_allocation(p, (paged_memory_t **)mem, lru_queue,
                                  sim_time);
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

bool paged_allocation(Process **p, paged_memory_t **mem, pqueue_t *lru_queue,
                      int sim_time) {

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
    int pages_required = ceil((float)(*p)->mem / PAGE_SIZE);
    // if pages not allocated, allocate frames
    if ((*mem)->frames_available >= pages_required) {
        paged_fit(p, mem, pages_required);
        return true;
    } else {

        // evict root of lru queue, if not enough, reheapify and evict root
        // again
        while ((*mem)->frames_available < pages_required) {
            Process *process_to_evict = peek(lru_queue);

            int pages_to_evict = ceil((float)process_to_evict->mem / PAGE_SIZE);
            // print evicted frames
            print_evicted_frames(process_to_evict, sim_time, pages_to_evict);

            // Actual eviction
            for (int i = 0; i < pages_to_evict; i++) {
                process_to_evict->pages[i].is_allocated = false;
                (*mem)
                    ->frames[process_to_evict->pages[i].frame_num]
                    .is_allocated = false;
                (*mem)->frames_available += 1;
            }
        }

        // now do paged fit
        paged_fit(p, mem, pages_required);
        return true;
    }
    return false;
}

void create_pages(Process **p) {
    (*p)->pages = (page_t *)malloc(sizeof(page_t) * ((*p)->mem / PAGE_SIZE));
    assert((*p)->pages != NULL);
    for (int i = 0; (float)i < (*p)->mem / PAGE_SIZE; i++) {
        (*p)->pages[i].frame_num = -1;
        (*p)->pages[i].is_allocated = false;
    }
}

bool virtual_allocation(Process **p, paged_memory_t **mem, pqueue_t *lru_queue,
                        int sim_time) {

    if ((*p)->pages == NULL) {
        create_pages(p);
    }

    int pages_required = 0;
    int pages_allocated = 0;

    for (int i = 0; i < ceil((float)(*p)->mem / (float)PAGE_SIZE); i++) {
        if (!(*p)->pages[i].is_allocated) {
            pages_required++;
        } else {
            pages_allocated++;
        }
    }

    // If the process already has enough allocated frames, return true
    if (pages_allocated >= pages_required) {
        return true;
    }

    if (pages_allocated >= 4) {
        // check if more frames are available
        if ((*mem)->frames_available >= pages_required) {
            paged_fit(p, mem, pages_required);
        } else {
            paged_fit(p, mem, (*mem)->frames_available);
        }
    } else {
        // less than 4 frames available, so allocate until 4 frames become
        // available
        int start_evict = 0;

        while ((*mem)->frames_available < 4) {
            Process *process_to_evict = peek(lru_queue);
            int pages_to_evict = 4 - (*mem)->frames_available;
            print_evicted_frames(process_to_evict, sim_time, pages_to_evict);

            // Find the first allocated page to start evict
            for (; start_evict < ceil((float)process_to_evict->mem / PAGE_SIZE);
                 start_evict++) {
                if (process_to_evict->pages[start_evict].is_allocated) {
                    break;
                }
            }

            // Evict pages
            for (int i = start_evict;
                 i < start_evict + pages_to_evict &&
                 i < ceil((float)process_to_evict->mem / (float)PAGE_SIZE);
                 i++) {
                if (process_to_evict->pages[i].is_allocated) {
                    process_to_evict->pages[i].is_allocated = false;
                    (*mem)
                        ->frames[process_to_evict->pages[i].frame_num]
                        .is_allocated = false;
                    (*mem)->frames_available += 1;
                }
            }

            // Add process back to queue if it still has pages to evict
            for (int i = 0;
                 i < ceil((float)process_to_evict->mem / (float)PAGE_SIZE);
                 i++) {
                if (process_to_evict->pages[i].is_allocated) {
                    insert(lru_queue, process_to_evict);
                    break;
                }
            }
        }

        // now do paged fit
        paged_fit(p, mem, pages_required);
    }

    return true;
}

// implement paged fit strategy
void paged_fit(Process **p, paged_memory_t **mem, int pages_to_fit) {
    int i = 0;
    int j = 0;
    while (i < (float)MEM_SIZE / (float)PAGE_SIZE && j < pages_to_fit) {
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
        case VIRTUAL:
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
    for (int i = 0; i < (float)p->mem / (float)PAGE_SIZE; i++) {
        (*mem)->frames[p->pages[i].frame_num].is_allocated = false;
        (*mem)->frames_available += 1;
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
        case VIRTUAL:
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

    int pages_allocated = 0;
    for (int i = 0; i < ceil((float)p->mem / (float)PAGE_SIZE); i++) {
        if (p->pages[i].is_allocated) {
            pages_allocated++;
        }
    }

    // Find the first allocated page
    int start = 0;
    for (int i = 0; i < ceil((float)p->mem / (float)PAGE_SIZE); i++) {
        if (p->pages[i].is_allocated) {
            start = i;
            break;
        }
    }

    printf(
        "%d,%s,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[",
        sim_time, get_state(p), p->name, p->rtime, get_paged_mem_usage(mem));
    for (int i = start; i < start + pages_allocated; i++) {
        if (i == start + pages_allocated - 1) {
            printf("%d]\n", p->pages[i].frame_num);
        } else {
            printf("%d,", p->pages[i].frame_num);
        }
    }
}

void print_evicted_frames(Process *p, int sim_time, int num_frames) {
    // Find the first allocated page
    int start = 0;
    for (int i = 0; i < ceil((float)p->mem / (float)PAGE_SIZE); i++) {
        if (p->pages[i].is_allocated) {
            start = i;
            break;
        }
    }

    printf("%d,EVICTED,evicted-frames=[", sim_time);
    int printed_frames = 0;
    for (int i = start; i < ceil((float)p->mem / (float)PAGE_SIZE) &&
                        printed_frames < num_frames;
         i++) {
        if (p->pages[i].is_allocated) {
            // print comma before the frame number, but not for the first frame
            if (printed_frames > 0) {
                printf(",");
            }
            printf("%d", p->pages[i].frame_num);
            printed_frames++;
        }
    }
    printf("]\n");
}

int get_block_mem_usage(block_memory_t *mem) {
    return ceil(100 - (((float)mem->available / (float)mem->size) * 100));
}

int get_paged_mem_usage(paged_memory_t *mem) {
    int page_used = NUM_PAGES - mem->frames_available;
    int percent_used = ceil((float)(page_used * 100) / (float)NUM_PAGES);
    return percent_used;
}