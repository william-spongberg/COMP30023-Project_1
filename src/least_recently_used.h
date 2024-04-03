#ifndef LEAST_RECENTLY_USED_H
#define LEAST_RECENTLY_USED_H

#include "process.h"

typedef struct Page {
    page_t *p;
    struct Page *next;
} Page;

// create a new LRU
Page *create_page(page_t *page);
void insert_page(Page **head, page_t *p);
Page *get_page(Page **head, page_t *p);
void page_used(Page **head, page_t *p);
Page *get_tail(Page **head);
void delete_page(Page **head, Page *page);
bool cmp_page(page_t *p1, page_t *p2);

#endif