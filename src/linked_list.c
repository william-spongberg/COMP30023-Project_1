#include "linked_list.h"

Node *createNode(Process *p) {
    Node *node = (Node *)malloc(sizeof(Node));
    assert(node != NULL);

    node->process = p;
    node->next = node; // point to self
    return node;
}

void insertNode(Node **head, Process *p) {
    Node *node = createNode(p);
    if (*head == NULL) {
        *head = node;
    } else {
        Node *temp = *head;
        // find node before head
        while (temp->next != *head)
            temp = temp->next;
        // insert node before head
        temp->next = node;
        node->next = *head;
    }
}

void deleteNode(Node **head, Process *p) {
    // if empty list
    if (*head == NULL) {
        printf("All processes finished\n");
        return;
    }

    // if deleting single head node
    if ((cmpProcess((*head)->process, p) == 0) && ((*head)->next == *head)) {
        free(*head);
        *head = NULL;
        return;
    }

    Node *current = *head;
    Node *prev = NULL;

    // if deleting head but not single head node
    if (cmpProcess((current)->process, p) == 0) {
        while (current->next != *head)
            current = current->next;
        current->next = (*head)->next;
        free(*head);
        *head = current->next;
    } else {
        // else traverse list to find the node to be deleted
        prev = current;
        current = current->next;
        while (current != *head) {
            if (cmpProcess((current)->process, p) == 0) {
                prev->next = current->next;
                free(current);
                break;
            }
            prev = current;
            current = current->next;
        }
    }
}

int listLength(Node *head) {
    if (head == NULL)
        return 0;

    int count = 0;
    Node *temp = head;
    do {
        count++;
        temp = temp->next;
    } while (temp != head);
    return count;
}

void printList(Node *head) {
    if (head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node *temp = head;
    do {
        printProcess(temp->process);
        printf("points to "); // %p\n", temp->next);
        printProcess(temp->next->process);
        temp = temp->next;
    } while (temp != head);
    printf("\n");
}