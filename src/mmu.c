#include "mmu.h"

// Create block of memory
memory_t *create_memory() {
    memory_t *mem = (memory_t *)malloc(sizeof(memory_t));
    mem->size = MEM_SIZE;
    mem->available = MEM_SIZE;
    mem->blocks = (int8_t *)calloc(MEM_SIZE, sizeof(int8_t));
    return mem;
}

// Implement first fit strategy

/*
    Helper:
    Return the index of the first bit in the memory block that 
    can fit the required size
*/
int first_available(memory_t *mem, int size) 
{
    int count = 0;
    int start = 0;
    for (int i = 0; i < mem->size; i++) 
    {
        if (mem->blocks[i] == 0) 
        {
            if (start == -1) 
            {
                start = i;
            }
            count++;
        } 
        else 
        {
            count = 0;
            start = -1;
        }

        if (count == size) {
            return start;
        }
    }
    return -1;
}

/* Actual first fit function */
int8_t first_fit(memory_t *mem, int size) 
{
    int start = first_available(mem, size);
    if (start == -1) 
    {
        return -1;
    }

    for (int i = start; i < start + size; i++) 
    {
        mem->blocks[i] = 1;
    }
    mem->available -= size;

    return start;
}


// Free memory
void free_memory(memory_t *mem, int start, int size) 
{
    for (int i = start; i < start + size; i++) 
    {
        mem->blocks[i] = 0;
    }
    mem->available += size;
}

// Clean up
void destroy_memory(memory_t *mem) 
{
    free(mem->blocks);
    free(mem);
}