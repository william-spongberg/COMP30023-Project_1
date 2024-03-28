#include <stdio.h>
#include <stdlib.h>

// Define queue structure
typedef struct queue {
    int front;
    int rear;
    int elem_count;
    int capacity;
    void **array;
} queue_t;

// Define queue functions
queue_t *create_queue(int capacity);
void enqueue(queue_t *queue, void *elem);
void *dequeue(queue_t *queue);
void *peek(queue_t *queue);
void free_queue(queue_t *queue);