#ifndef QUEUE_H
#define QUEUE_H
#include "process.h"

// circular linked list
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

// method signatures
Node *create_node(Process *p);
void insert_node(Node **head, Process *p);
void delete_node(Node **head, Process *p);
int list_length(Node *head);
void print_list(Node *head);

#endif