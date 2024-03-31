#include "process.h"

// create process
Process *createProcess(int time, char *name, int rtime, int mem, pState state) {
    Process *p = (Process *)malloc(sizeof(Process));
    assert(p != NULL);

    p->time = time;
    strcpy(p->name, name);
    p->rtime = rtime;
    p->mem = mem;
    p->state = state;
    return p;
}

// compare process names
int cmpProcess(Process *p1, Process *p2) {
    return strcmp(p1->name, p2->name);
}

// get process state
char *getState(Process *p) {
    switch (p->state) {
    case READY:
        return "READY";
        break;
    case RUNNING:
        return "RUNNING";
        break;
    case FINISHED:
        return "FINISHED";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

// print process info
void printProcess(Process *p) {
    printf("%d,%s,process-name=%s,remaining-time=%d\n", p->time, getState(p),
           p->name, p->rtime);
}