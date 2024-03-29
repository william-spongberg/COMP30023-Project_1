#include <stdio.h>
#include <stdlib.h>

/* Be careful when using this queue module, to reduce unnecessary coupling, the queue module is implemented
    to be polymorphic, hence when inserting processes, you must insert the pointer to the process created
    rather than the process itself. If this turned out to have unnecessarily made the program more complicated,
    it should be refactored to be used for process structure only */

// Define queue structure
typedef struct queue {
    int front;
    int rear;
    int elem_count;
    int capacity;
    void **array;
} queue_t;

// Define queue functions
queue_t *create_queue();
void enqueue(queue_t *queue, void *elem);

// Return the element for freeing memory after dequeue
void *dequeue(queue_t *queue);

// Return the first element in the queue without removing it
void *peek(queue_t *queue);

// Assume all processes are run to finish and memory freed, simply free the queue
void free_queue(queue_t *queue);