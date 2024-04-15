#include "queue.h"

Node *create_node(Process *p) {
    Node *node = (Node *)malloc(sizeof(Node));
    assert(node != NULL);

    node->process = p;
    node->next = node; // point to self
    return node;
}

void insert_node(Node **head, Process **p) {
    Node *node = create_node(*p);
    if (*head == NULL) {
        *head = node;
    } else {
        Node *temp = *head;
        // check if process is already in the list
        do {
            // if process is already in the list, free node and process
            if (temp != NULL && cmp_process(temp->process, *p) == 0) {
                free(node);
                free(*p);
                *p = NULL;
                return;
            }
            temp = temp->next;
        } while (temp != *head);

        // find node before head
        temp = *head;
        while (temp->next != *head)
            temp = temp->next;
        // insert node before head
        temp->next = node;
        node->next = *head;
    }
}

void delete_node(Node **head, Process *p) {
    // if empty list
    if (*head == NULL) {
        printf("All processes finished\n");
        return;
    }

    // if deleting single head node
    if ((cmp_process((*head)->process, p) == 0) && ((*head)->next == *head)) {
        free(*head);
        *head = NULL;
        return;
    }

    Node *current = *head;
    Node *prev = NULL;

    // if deleting head but not single head node
    if (cmp_process((current)->process, p) == 0) {
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
            if (cmp_process((current)->process, p) == 0) {
                prev->next = current->next;
                free(current);
                break;
            }
            prev = current;
            current = current->next;
        }
    }
}

int list_length(Node *head) {
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

void print_list(Node *head) {
    if (head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node *temp = head;
    do {
        print_running_process(temp->process, 0);
        printf("points to ");
        print_running_process(temp->next->process, 0);
        temp = temp->next;
    } while (temp != head);
    printf("\n");
}