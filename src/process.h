#ifndef process_H
#define process_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { READY, RUNNING, FINISHED } pState;

typedef struct Process {
    int time;     // time process enters CPU
    char name[8]; // name of process
    int rtime;    // remaining time
    int mem;      // amount of memory required
    pState state; // current state of process
} Process;

// method signatures
Process *createProcess(int time, char *name, int rtime, int mem, pState state);
int cmpProcess(Process *p1, Process *p2);
char *getState(Process *p);
void printProcess(Process *p);

#endif