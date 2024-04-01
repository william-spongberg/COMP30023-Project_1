#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include "process_scheduler.h"
#include <stdbool.h>

void run_simulation(char *filename, char *memory_strategy, int quantum);
bool load_processes(Node **linked_list, char *filename, int sim_time,
                    int quantum);
void finish_process(Node **node, Process **p);
void increment_sim_time(Process **p, int *sim_time, int quantum);
#endif