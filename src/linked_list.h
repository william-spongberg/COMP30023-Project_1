#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "process.h"

// circular linked list
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

// method signatures
Node *createNode(Process *p);
void insertNode(Node **head, Process *p);
void deleteNode(Node **head, Process *p);
int listLength(Node *head);
void printList(Node *head);

#endif