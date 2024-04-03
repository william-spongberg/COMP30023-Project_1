#include "least_recently_used.h"

// new linked list
Page *create_page(page_t *p) {
    Page *page = (Page *)malloc(sizeof(Page));
    assert(page != NULL);

    page->p = p;
    page->next = NULL;
    return page;
}

// insert page
void insert_page(Page **head, page_t *p) {
    Page *page = create_page(p);
    if (*head == NULL) {
        *head = page;
    } else {
        Page *temp = *head;
        // insert at tail, least recently used
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = page;
    }
}

Page *get_page(Page **head, page_t *p) {
    Page *temp = *head;
    while (temp != NULL) {
        if (cmp_page(temp->p, p))
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// if page is used, move to head
void page_used(Page **head, page_t *p) {
    Page *page = get_page(head, p);
    if (page == NULL)
        return;
    // if page is head, do nothing
    if (cmp_page((*head)->p, p))
        return;
    // if page is tail, move to head
    if (page->next == NULL) {
        page->next = *head;
        *head = page;
        return;
    }
    // if page is in middle, move to head
    Page *temp = *head;
    while (temp->next != page)
        temp = temp->next;
    temp->next = page->next;
    page->next = *head;
    *head = page;
}

Page *get_tail(Page **head) {
    Page *temp = *head;
    while (temp->next != NULL)
        temp = temp->next;
    return temp;
}

void delete_page(Page **head, Page *page) {
    Page *temp = *head;
    if (cmp_page(temp->p, page->p)) {
        *head = temp->next;
        free(temp);
        return;
    }
    while (temp->next != NULL) {
        if (cmp_page(temp->next->p, page->p)) {
            Page *del = temp->next;
            temp->next = temp->next->next;
            free(del);
            return;
        }
        temp = temp->next;
    }
}

bool cmp_page(page_t *p1, page_t *p2) {
    if (p1 == NULL || p2 == NULL)
        return false;
    if (p1->frame_num == -1)
        return false;
    return p1->frame_num == p2->frame_num;
}