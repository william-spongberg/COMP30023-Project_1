#include "least_recently_used.h"

// Create a new priority queue
pqueue_t *create_pqueue() {
    pqueue_t *pq = (pqueue_t *)malloc(sizeof(pqueue_t));
    assert(pq != NULL);
    pq->processes = (Process **)malloc(INIT_CAP * sizeof(Process *));
    assert(pq->processes != NULL);
    pq->size = 0;
    pq->capacity = INIT_CAP;
    return pq;
}

// Insert a process into the heap
void insert(pqueue_t *pq, Process *p) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->processes = (Process **)realloc(pq->processes,
                                            pq->capacity * sizeof(Process *));
        assert(pq->processes != NULL);
    }

    pq->processes[pq->size] = p;
    pq->size++;
    heapify(pq);
}

// sift down current root in the heap
void sift_down(pqueue_t *pq, int i) {
    int min = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < pq->size && cmp_priority(pq->processes[left], pq->processes[min]) < 0) {
        min = left;
    }

    if (right < pq->size && cmp_priority(pq->processes[right], pq->processes[min]) < 0) {
        min = right;
    }

    if (min != i) {
        Process *temp = pq->processes[i];
        pq->processes[i] = pq->processes[min];
        pq->processes[min] = temp;
        sift_down(pq, min);
    }
}

// heapify a random array
void heapify(pqueue_t *pq) {
    // print out all process name in the heap
    if (pq == NULL || pq->size == 0) {
        return;
    }
    for (int i = (pq->size / 2) - 1; i >= 0; i--) {
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
    heapify(pq);
    return root;
}

// cmp_priority compares the last execution time of two processes
int cmp_priority(Process *p1, Process *p2) {
    if (p1 == NULL || p2 == NULL) {
        return 0;
    }
    return (p1->last_exec) - (p2->last_exec);
}

// return if a process is in the queue
bool in_queue(pqueue_t *pq, Process *p) {
    if (pq->size == 0) {
        return false;
    }
    if (p == NULL) {
        return false;
    }
    for (int i = 0; i < pq->size; i++) {
        if (pq->processes[i] == NULL) {
            continue;
        }
        if (cmp_process(pq->processes[i], p) == 0) {
            return true;
        }
    }
    return false;
}

void delete_process(pqueue_t *pq, Process *p) {
    if (pq->size == 0) {
        return;
    }
    if (p == NULL) {
        return;
    }
    for (int i = 0; i < pq->size; i++) {
        if (pq->processes[i] == NULL) {
            continue;
        }
        if (cmp_process(pq->processes[i], p) == 0) {
            pq->processes[i] = NULL;
            pq->size--;
            heapify(pq);
            return;
        }
    }
}

void free_pqueue(pqueue_t *pq) {
    free(pq->processes);
    pq->processes = NULL;
    free(pq);
    pq = NULL;
}