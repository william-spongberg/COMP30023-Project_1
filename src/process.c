#include "process.h"

// create process
Process *create_process(int time, char *name, int rtime, int mem,
                        p_state state) {
    Process *p = (Process *)malloc(sizeof(Process));
    assert(p != NULL);

    p->time = time;
    strcpy(p->name, name);
    p->rtime = rtime;
    p->mem = mem;
    p->addr = -1;
    p->state = state;
    return p;
}

// compare process names
int cmp_process(Process *p1, Process *p2) {
    return strcmp(p1->name, p2->name);
}

// get process state
char *get_state(Process *p) {
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
void print_running_process(Process *p, int sim_time) {
    printf("%d,%s,process-name=%s,remaining-time=%d\n", sim_time, get_state(p),
           p->name, p->rtime);
}

void print_finished_process(Process *p, int sim_time, int list_length) {
    printf("%d,%s,process-name=%s,proc-remaining=%d\n", sim_time, get_state(p),
           p->name, list_length);
}