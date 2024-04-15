#ifndef LEAST_RECENTLY_USED_H
#define LEAST_RECENTLY_USED_H

#include "process.h"

#define INIT_CAP 10

// data structure for priority queue
typedef struct pqueue {
    Process **processes;
    int size;
    int capacity;
} pqueue_t;

// method signatures
void heapify(pqueue_t *pq);
pqueue_t *create_pqueue();
Process *dequeue(pqueue_t *pq);
void enqueue(pqueue_t *pq, Process *p);
bool in_queue(pqueue_t *pq, Process *p);

int cmp_priority(Process *p1, Process *p2);
void delete_process(pqueue_t *pq, Process *p);
void free_pqueue(pqueue_t *pq);

#endif