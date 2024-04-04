#ifndef LEAST_RECENTLY_USED_H
#define LEAST_RECENTLY_USED_H

#include "process.h"

#define INIT_CAP 10

// data structure for priority queue to store processes in their last execution time
typedef struct pqueue{
    Process **processes;
    int size;
    int capacity;
} pqueue_t;

void heapify(pqueue_t *pq);

// Create a new priority queue
pqueue_t *create_pqueue();

// Insert a process into the heap
void insert(pqueue_t *pq, Process *p);

// Get the root of the heap, delete it and return it
Process *peek(pqueue_t *pq);

// determine priority of processes to be evicted, the one with the least recent execution time
int cmp_priority(Process *p1, Process *p2);

// return if a process is in the queue
bool in_queue(pqueue_t *pq, Process *p);

#endif