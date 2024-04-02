#include "memory_manager.h"

// TODO: set memory strategies
void set_memory_strategy(char *memory_strategy) {
    if (strcmp(memory_strategy, "infinite") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "first-fit") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "paged") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "virtual") == 0) {
        // set strategy
    } else {
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
}

// create block of memory
memory_t *create_memory(char *memory_strategy) {
    if (strcmp(memory_strategy, "infinite") == 0) {
        return NULL;
    } else if (strcmp(memory_strategy, "first-fit") == 0) {
        memory_t *mem = (memory_t *)malloc(sizeof(memory_t));
        mem->size = MEM_SIZE;
        mem->available = MEM_SIZE;
        for (int i = 0; i < MEM_SIZE; i++) {
            mem->blocks[i] = 0;
        }
        return mem;
    }
    return NULL;
}

// allocate memory
int16_t allocate_memory(memory_t *mem, char *memory_strategy, int size) {
    if (strcmp(memory_strategy, "infinite") == 0) {
        return -1;
    } else if (strcmp(memory_strategy, "first-fit") == 0) {
        return first_fit(mem, size);
    }
    fprintf(stderr, "Invalid memory strategy\n");
    exit(EXIT_FAILURE);
}

// implement first fit strategy
int16_t first_fit(memory_t *mem, int size) {
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
void free_memory(memory_t *mem, int start, int size) {
    if (mem != NULL) {
        for (int i = start; i < start + size; i++) {
            mem->blocks[i] = 0;
        }
        mem->available += size;
    }
}

// clean up
void destroy_memory(memory_t *mem) {
    if (mem != NULL)
        free(mem);
}

// print memory usage
int get_mem_usage(memory_t *mem) {
    return ceil(100 - (((float)mem->available / (float)MEM_SIZE) * 100));
}

void print_memory_process(Process *p, memory_t *mem, int sim_time) {
    if (mem == NULL) {
        fprintf(stderr, "mem is NULL\n");
        exit(EXIT_FAILURE);
    }
    printf(
        "%d,%s,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%"
        "d\n",
        sim_time, get_state(p), p->name, p->rtime, get_mem_usage(mem), p->addr);
}