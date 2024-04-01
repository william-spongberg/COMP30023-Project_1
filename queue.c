#include "queue.h"

#define INIT_CAPACITY 5

// Implement queue functions
queue_t *create_queue() {
    // Initiate pointer to the queue structure
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));

    // Default values for the queue structure
    queue->front = 0;
    queue->capacity = INIT_CAPACITY;
    queue->rear = 0;
    queue->elem_count = 0;

    // Allocate memory for the array of pointers
    queue->array = (void **)malloc(queue->capacity * sizeof(void *));
    return queue;
}

void enqueue(queue_t *queue, void *elem) {
    // Check if the queue is full
    if (queue->elem_count == queue->capacity) {
        // If full, double the capacity
        queue->capacity *= 2;
        queue->array = (void **)realloc(queue->array, queue->capacity * sizeof(void *));
    }

    // Insert at rear as circular queue
    queue->array[queue->rear] = elem;
    // Increment rear and ensure it is within bounds
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->elem_count++;
}

void *dequeue(queue_t *queue) {
    // Check if the queue is empty
    if (queue->elem_count == 0) {
        return NULL;
    }

    // Get the element at the front
    void *elem = queue->array[queue->front];
    // Increment front and ensure it is within bounds
    queue->front = (queue->front + 1) % queue->capacity;
    queue->elem_count--;
    return elem;
}

void *peek(queue_t *queue) {
    // Check if the queue is empty
    if (queue->elem_count == 0) {
        return NULL;
    }

    // Return the element at the front
    return queue->array[queue->front];
}

void free_queue(queue_t *queue) {
    // Free the array of pointers
    free(queue->array);
    // Free the queue structure
    free(queue);
}