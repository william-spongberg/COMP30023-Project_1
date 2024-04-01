#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H
#include "linked_list.h"

// method signatures
void roundRobinScheduling(Process **processes, int n, int quantum);
void printFinishedProcess(int sim_time, Process *p, int list_length);
void printRunningProcess(Process *p, int sim_time);

#endif