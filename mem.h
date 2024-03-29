#include <stdio.h>
#include <stdlib.h>

// define enum of memory strategies
typedef enum {
    INFINITE,
    FIRST_FIT,
    PAGED,
    VIRTUAL
} memory_strategy_t;

// define wrapper memory functions for each strategy
// FIXME: Determine whether return process or take process as argument
void allocate_memory(int size, memory_strategy_t strategy);