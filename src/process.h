#ifndef process_H
#define process_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4 // KB

typedef enum { READY, RUNNING, FINISHED } p_state;

typedef struct page_t {
    int frame_num;
    bool is_allocated;
} page_t;

typedef struct Process {
    int arrival_time; // time process enters CPU
    int service_time; // time process exits CPU
    char name[8];     // name of process
    int rtime;        // remaining arrival_time
    int mem;          // amount of memory required
    int last_exec;    // last time process was executed
    int16_t addr;     // address of memory allocated
    p_state state;    // current state of process
    page_t *pages;    // pages allocated as array of pages
} Process;

// method signatures
Process *create_process(int arrival_time, char *name, int rtime, int mem,
                        p_state state);
int cmp_process(Process *p1, Process *p2);
char *get_state(Process *p);
int get_page_count(Process *p);

void print_running_process(Process *p, int sim_time);
void print_finished_process(Process *p, int sim_time, int list_length);
void free_process(Process **p);

#endif