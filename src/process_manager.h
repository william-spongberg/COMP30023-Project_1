#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include "process_scheduler.h"

void run_simulation(char *filename, mem_strategy strategy, int quantum);
bool process_completed(Process *p);

void print_process(Process *p, block_memory_t *mem, mem_strategy strategy,
                   int sim_time);
void run_process(Process **p, block_memory_t *mem, p_state *curr_state,
                 mem_strategy mem_strategy, int sim_time);
void finish_process(Node **node, Process **p, void **mem,
                    mem_strategy strategy);
void increment_sim_time(Process **p, int *sim_time, int quantum);
#endif