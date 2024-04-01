#include "process_scheduler.h"

void round_robin_scheduling(Node **queue, Process **p, int *sim_time) {
    // run available processes
    if (*queue != NULL) {
        // if no running process, get current process
        if (*p == NULL) {
            *p = (*queue)->process;
            (*p)->state = RUNNING;
            print_running_process(*p, *sim_time);
        } else if (*queue != (*queue)->next) {
            // if not only process, set to READY and switch
            (*p)->state = READY;
            *queue = (*queue)->next;

            *p = (*queue)->process;
            (*p)->state = RUNNING;
            print_running_process(*p, *sim_time);
        }
    }
}
