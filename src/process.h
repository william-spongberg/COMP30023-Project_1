#ifndef process_H
#define process_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { READY, RUNNING, FINISHED } p_state;

typedef struct Process {
    int time;     // time process enters CPU
    char name[8]; // name of process
    int rtime;    // remaining time
    int mem;      // amount of memory required
    p_state state; // current state of process
} Process;

// method signatures
Process *create_process(int time, char *name, int rtime, int mem, p_state state);
int cmp_process(Process *p1, Process *p2);
char *get_state(Process *p);
void print_process(Process *p);

#endif