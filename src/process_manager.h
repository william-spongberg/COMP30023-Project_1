#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include "process_scheduler.h"

void run_simulation(char *filename, char *memory_strategy, int quantum);
bool load_processes(Node **linked_list, char *filename, int sim_time,
                    int quantum);
bool process_completed(Process *p);
void print_process(Process *p, memory_t *mem, char *memory_strategy,
                   int sim_time);
void run_process(Process **p, memory_t *mem, p_state *curr_state,
                 char *memory_strategy, int sim_time);
void finish_process(Node **node, Process **p, memory_t **mem);
void increment_sim_time(Process **p, int *sim_time, int quantum);
#endif