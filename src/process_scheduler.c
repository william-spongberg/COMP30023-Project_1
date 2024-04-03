#include "process_scheduler.h"

void round_robin_scheduling(Node **queue, Process **p, p_state *curr_state) {
    // run available processes
    if (*queue != NULL) {
        // if no running process, get current process
        if (*p == NULL) {
            *p = (*queue)->process;
        } else if (*queue != (*queue)->next) {
            // if not only process, set to READY and switch
            (*p)->state = READY;
            *curr_state = (*p)->state;
            *queue = (*queue)->next;
            *p = (*queue)->process;
        }
    }
}
