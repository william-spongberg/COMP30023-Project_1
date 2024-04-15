#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "process_scheduler.h"

// statistics struct
typedef struct stats_t {
    int num_processes;
    int total_turnaround_time;
    float total_time_overhead;
    float max_time_overhead;
} stats_t;

// method signatures
void run_simulation(char *filename, mem_strategy strategy, int quantum);
bool processes_remaining(Node **queue, char *filename, int *num_lines,
                         int sim_time, int quantum);
void increment_sim_time(Process **p, int *sim_time, int quantum);

void run_process(Process **p, void *mem, p_state *curr_state,
                 mem_strategy strategy, int sim_time, pqueue_t *lru_queue);
void finish_process(Node **node, Process **p, void **mem, mem_strategy strategy,
                    pqueue_t *lru_queue, int sim_time, p_state *curr_state,
                    stats_t *stats);
bool is_process_finished(Process *p);
void print_process(Process *p, void *mem, mem_strategy strategy, int sim_time);

stats_t new_stats();
void print_stats(stats_t stats, int sim_time);

void free_memory_structs(void **mem, pqueue_t **lru_queue);

#endif