#include "memory_manager.h"

// create memory struct
void *create_memory(mem_strategy strategy) {
    switch (strategy) {
    case INFINITE:
        // no memory to create
        return NULL;
    case FIRST_FIT:
        return create_block_memory();
    case PAGED:
    case VIRTUAL:
        return create_paged_memory();
    default:
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

// create block memory
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

// create paged memory
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

// attempt to allocate memory according to strategy
bool attempt_alloc(Process **p, void **mem, mem_strategy strategy,
                   pqueue_t *lru_queue, int sim_time) {
    if (mem == NULL)
        return true;
    switch (strategy) {
    case INFINITE:
        // don't need to allocate memory
        return true;
    case FIRST_FIT:
        return first_fit_alloc(p, *((block_memory_t **)mem));
    case PAGED:
        return paged_alloc(p, (paged_memory_t **)mem, lru_queue, sim_time);
    case VIRTUAL:
        return virtual_alloc(p, (paged_memory_t **)mem, lru_queue, sim_time);
    default:
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
    // default to true
    return true;
}

// allocate memory using first fit strategy
bool first_fit_alloc(Process **p, block_memory_t *mem) {
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

// find first fit for memory allocation
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

// allocate memory using paged strategy
bool paged_alloc(Process **p, paged_memory_t **mem, pqueue_t *lru_queue,
                 int sim_time) {
    // if pages not allocated, create pages
    if ((*p)->pages == NULL) {
        create_pages(p);
    }

    // if all pages are allocated, return true
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
    int pages_required = ceil((float)(*p)->mem / PAGE_SIZE);
    if ((*mem)->frames_available >= pages_required) {
        paged_fit(p, mem, pages_required);
        return true;
    } else {
        // evict root of lru queue until enough frames available
        while ((*mem)->frames_available < pages_required) {
            Process *process_to_evict = dequeue(lru_queue);

            // print evicted frames
            int pages_to_evict = ceil((float)process_to_evict->mem / PAGE_SIZE);
            print_evicted_frames(process_to_evict, sim_time, pages_to_evict);

            // evict pages
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

// create pages for process
void create_pages(Process **p) {
    (*p)->pages = (page_t *)malloc(sizeof(page_t) *
                                   (ceil((float)(*p)->mem / (float)PAGE_SIZE)));
    assert((*p)->pages != NULL);
    for (int i = 0; i < ceil(((float)(*p)->mem / (float)PAGE_SIZE)); i++) {
        (*p)->pages[i].frame_num = -1;
        (*p)->pages[i].is_allocated = false;
    }
}

// implement paged fit strategy
void paged_fit(Process **p, paged_memory_t **mem, int pages_to_fit) {
    int i = 0;
    int j = 0;
    // insert pages into available frames
    while (i < (float)MEM_SIZE / (float)PAGE_SIZE && j < pages_to_fit) {
        if (!(*mem)->frames[i].is_allocated) {
            (*p)->pages[j].frame_num = i;
            (*p)->pages[j].is_allocated = true;
            (*mem)->frames[i].is_allocated = true;
            (*mem)->frames_available -= 1;
            j++;
        }
        i++;
    }
}

// allocate memory using virtual strategy
bool virtual_alloc(Process **p, paged_memory_t **mem, pqueue_t *lru_queue,
                   int sim_time) {

    // if pages not allocated, create pages
    if ((*p)->pages == NULL) {
        create_pages(p);
    }

    // get number of pages required/allocated
    int pages_required = 0;
    int pages_allocated = 0;
    for (int i = 0; i < ceil((float)(*p)->mem / PAGE_SIZE); i++) {
        if (!(*p)->pages[i].is_allocated) {
            pages_required++;
        } else {
            pages_allocated++;
        }
    }

    // if already enough allocated frames, return true
    if (pages_allocated >= pages_required || pages_allocated >= 4) {
        return true;
    } else {
        // less than 4 frames available, so allocate until available
        int start_evict = 0;

        while ((*mem)->frames_available < 4) {
            Process *process_to_evict = dequeue(lru_queue);

            // print evicted frames
            int pages_to_evict = 4 - (*mem)->frames_available;
            print_evicted_frames(process_to_evict, sim_time, pages_to_evict);

            // find the first allocated page to start evict
            for (; start_evict < ceil((float)process_to_evict->mem / PAGE_SIZE);
                 start_evict++) {
                if (process_to_evict->pages[start_evict].is_allocated) {
                    break;
                }
            }

            // evict pages
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

            // add process back to queue if it still has pages to evict
            for (int i = 0;
                 i < ceil((float)process_to_evict->mem / (float)PAGE_SIZE);
                 i++) {
                if (process_to_evict->pages[i].is_allocated) {
                    enqueue(lru_queue, process_to_evict);
                    break;
                }
            }
        }
        // now do paged fit
        paged_fit(p, mem, pages_required);
    }

    return true;
}

// free memory based on strategy
void free_process_memory(void **mem, Process **p, mem_strategy strategy,
                         int start, int size) {
    if (mem != NULL) {
        switch (strategy) {
        case INFINITE:
            // no memory to free
            break;
        case FIRST_FIT:
            free_block_memory(*(block_memory_t **)mem, start, size);
            break;
        case PAGED:
        case VIRTUAL:
            free_paged_memory((paged_memory_t **)mem, *p);
            break;
        default:
            fprintf(stderr, "invalid memory strategy\n");
            exit(EXIT_FAILURE);
        }
    }
}

// free block memory
void free_block_memory(block_memory_t *mem, int start, int size) {
    for (int i = start; i < start + size; i++) {
        mem->blocks[i] = 0;
    }
    mem->available += size;
}

// free paged memory
void free_paged_memory(paged_memory_t **mem, Process *p) {
    for (int i = 0; i < (float)p->mem / (float)PAGE_SIZE; i++) {
        if (p->pages[i].is_allocated) {
            (*mem)->frames[p->pages[i].frame_num].is_allocated = false;
            (*mem)->frames_available += 1;
        }
    }
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

    // find first allocated page
    int start = get_first_allocated_frame(p);
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

// print evicted frames
void print_evicted_frames(Process *p, int sim_time, int num_frames) {
    // find first allocated page
    int start = get_first_allocated_frame(p);
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

// get block memory usage
int get_block_mem_usage(block_memory_t *mem) {
    return ceil(100 - (((float)mem->available / (float)mem->size) * 100));
}

// get paged memory usage
int get_paged_mem_usage(paged_memory_t *mem) {
    int page_used = NUM_PAGES - mem->frames_available;
    int percent_used = ceil((float)(page_used * 100) / (float)NUM_PAGES);
    return percent_used;
}

// get first allocated frame of process
int get_first_allocated_frame(Process *p) {
    for (int i = 0; i < ceil((float)p->mem / (float)PAGE_SIZE); i++) {
        if (p->pages[i].is_allocated) {
            return i;
        }
    }
    // should never reach here
    exit(EXIT_FAILURE);
    fprintf(stderr, "No pages allocated\n");
    return -1;
}