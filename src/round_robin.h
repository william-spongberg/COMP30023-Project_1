#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H
#include "linked_list.h"

// method signatures
void round_robin_scheduling(Process **processes, int n, int quantum);
void print_finished_process(int sim_time, Process *p, int list_length);
void print_running_process(Process *p, int sim_time);

#endif