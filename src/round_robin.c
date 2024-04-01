#include "round_robin.h"

void round_robin_scheduling(Process **processes, int n, int quantum) {
    Node *current = NULL;
    Process *p = NULL;
    int sim_time = 0;
    int proc_remaining = n;
    int j = 0;

    // run simulation
    while (proc_remaining > 0) {
        // add processses to circular linked list as they arrive
        for (int i = j; i < n; i++) {
            // insert processes at correct arrival time
            if (processes[i]->time <= sim_time) {
                insert_node(&current, processes[i]);
                p = processes[i];
                j = i + 1;

                // if only process, set to RUNNING
                if (list_length(current) == 1) {
                    p->state = RUNNING;
                    print_running_process(p, sim_time);
                    // TODO: refactor this
                    if (p->rtime > quantum) {
                        // increment simulation time
                        p->rtime -= quantum;
                        sim_time += quantum;
                    }
                } else {
                    p->state = READY;
                }
            }
        }

        // run available processes
        if (current != NULL) {
            // get new process
            if (p == NULL) {
                p = current->process;
                p->state = RUNNING;
                print_running_process(p, sim_time);
            } else if (current != current->next) {
                // if not only process, set to READY and switch
                p->state = READY;

                current = current->next;
                p = current->process;
                p->state = RUNNING;
                print_running_process(p, sim_time);
            }

            // run the process
            if (p->rtime > quantum) {
                // increment simulation time
                p->rtime -= quantum;
                sim_time += quantum;
            } else {
                // increment simulation time
                sim_time += quantum;

                // if done using CPU, delete process
                p->rtime = 0;
                p->state = FINISHED;
                proc_remaining--;
                print_finished_process(sim_time, p, list_length(current) - 1);

                // delete process
                delete_node(&current, p);
                free(p);
                p = NULL;
            }
        } else {
            // run CPU idly
            sim_time += quantum;
        }
    }
}

void print_finished_process(int sim_time, Process *p, int list_length) {
    printf("%d,%s,process-name=%s,proc-remaining=%d\n", sim_time, get_state(p),
           p->name, list_length);
}

void print_running_process(Process *p, int sim_time) {
    printf("%d,%s,process-name=%s,remaining-time=%d\n", sim_time, get_state(p),
           p->name, p->rtime);
}
