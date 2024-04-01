#include "mem.h"

// Define helper functions for each memory strategy
// FIXME: Add appropriate arguments to each function
void infinite();
void first_fit();
void paged();
void virtual();

// Implement memory allocation function
// FIXME: Add appropriate arguments to function signature
void allocate_memory(int size, memory_strategy_t strategy) {
    switch (strategy) {
        case INFINITE:
            infinite();
            break;
        case FIRST_FIT:
            first_fit();
            break;
        case PAGED:
            paged();
            break;
        case VIRTUAL:
            virtual();
            break;
    }
    return NULL;
}

// TODO: Implement individual memory strategy functions
void infinite() {
    // Implement infinite memory allocation strategy
}

void first_fit() {
    // Implement first fit memory allocation strategy
}

void paged() {
    // Implement paged memory allocation strategy
}

void virtual() {
    // Implement virtual memory allocation strategy
}

