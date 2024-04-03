#ifndef process_H
#define process_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGE_SIZE 4   // KB

typedef enum { READY, RUNNING, FINISHED } p_state;

typedef struct page_t{
    int frame_num;
    bool is_allocated;
} page_t;

typedef struct Process {
    int time;      // time process enters CPU
    char name[8];  // name of process
    int rtime;     // remaining time
    int mem;       // amount of memory required
    int16_t addr;  // address of memory allocated
    p_state state; // current state of process
    page_t *pages; // pages allocated
} Process;

// method signatures
Process *create_process(int time, char *name, int rtime, int mem,
                        p_state state);
int cmp_process(Process *p1, Process *p2);
char *get_state(Process *p);
void print_running_process(Process *p, int sim_time);
void print_finished_process(Process *p, int sim_time, int list_length);

#endif