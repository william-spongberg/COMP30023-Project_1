#include "least_recently_used.h"

// Create a new priority queue
pqueue_t *create_pqueue() {
    pqueue_t *pq = (pqueue_t *)malloc(sizeof(pqueue_t));
    pq->processes = (Process **)malloc(INIT_CAP * sizeof(Process *));
    pq->size = 0;
    pq->capacity = INIT_CAP;
    return pq;
}

// Insert a process into the heap
void insert(pqueue_t *pq, Process *p) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->processes = (Process **)realloc(pq->processes, pq->capacity * sizeof(Process *));
    }

    pq->processes[pq->size] = p;
    pq->size++;
    heapify(pq);
}

// sift down a process in the heap
void sift_down(pqueue_t *pq, int i) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int smallest = i;

    if (left < pq->size && cmp_priority(pq->processes[left], pq->processes[smallest]) < 0) {
        smallest = left;
    }
    if (right < pq->size && cmp_priority(pq->processes[right], pq->processes[smallest]) < 0) {
        smallest = right;
    }
    if (smallest != i) {
        Process *temp = pq->processes[i];
        pq->processes[i] = pq->processes[smallest];
        pq->processes[smallest] = temp;
        sift_down(pq, smallest); // recursively sift down
    }
}

// heapify a random array
void heapify(pqueue_t *pq) {
    if (pq == NULL) {
        return;
    }

    for (int i = pq->size / 2 - 1; i >= 0; i--) {
        sift_down(pq, i);
    }
}

// Peek
Process *peek(pqueue_t *pq) {
    if (pq->size == 0) {
        return NULL;
    }

    Process *root = pq->processes[0];
    pq->processes[0] = pq->processes[pq->size - 1];
    pq->size--;
    sift_down(pq, 0);
    return root;
}

// cmp_priority compares the last execution time of two processes
int cmp_priority(Process *p1, Process *p2) {
    return p1->last_exec - p2->last_exec;
}

// return if a process is in the queue
bool in_queue(pqueue_t *pq, Process *p) {
    if (pq->size == 0) {
        return false;
    }
    for (int i = 0; i < pq->size; i++) {
        if (cmp_process(pq->processes[i], p) == 0) {
            return true;
        }
    }
    return false;
}