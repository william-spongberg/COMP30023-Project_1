#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "process_scheduler.h"

typedef struct stats_t {
    int num_processes;
    int total_turnaround_time;
    float total_time_overhead;
    float max_time_overhead;
} stats_t;

void run_simulation(char *filename, mem_strategy strategy, int quantum);
bool process_completed(Process *p);

void print_process(Process *p, void *mem, mem_strategy strategy,
                   int sim_time);
void run_process(Process **p, void *mem, p_state *curr_state,
                 mem_strategy strategy, int sim_time);
void finish_process(Node **node, Process **p, void **mem,
                    mem_strategy strategy, int sim_time, stats_t *stats);
void increment_sim_time(Process **p, int *sim_time, int quantum);
stats_t new_stats();

#endif