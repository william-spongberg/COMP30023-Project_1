#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H
#include "memory_manager.h"

// method signatures
void round_robin_scheduling(Node **queue, Process **p, p_state *curr_state);

#endif