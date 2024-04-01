#include "process_manager.h"

void run_simulation(char *filename, char *memory_strategy, int quantum) {
    Node *queue = NULL;
    Process *p = NULL;
    int sim_time = 0;

    // while processes are being loaded or there are still processes to run
    while (load_processes(&queue, filename, sim_time, quantum) ||
           (queue != NULL)) {

        // if process has completed its execution, delete it
        if (p != NULL && queue != NULL && p->rtime == 0) {
            p->state = FINISHED;
            print_finished_process(p, sim_time, list_length(queue) - 1);
            finish_process(&queue, &p);
        }

        // update state of current process, state of newly allocated process
        round_robin_scheduling(&queue, &p, &sim_time);

        // increment simulation time
        increment_sim_time(&p, &sim_time, quantum);
    }
}

// TODO: allow for task2, task3, task4 files
bool load_processes(Node **queue, char *filename, int sim_time, int quantum) {
    // open file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    // format: time process-name remaining-time memory
    // e.g. 0 P1 10 100
    char name[8];
    int time, rtime;
    int mem = 0;

    // read file
    while (fscanf(file, "%d %s %d %d", &time, name, &rtime, &mem) != EOF) {
        // if arrived since last cycle, set to READY and add to linked list
        // TODO: fix so works according to doc
        //if ((time >= (sim_time - quantum)) && (time <= sim_time)) {
        if (time == sim_time) {
            insert_node(queue, create_process(time, name, rtime, mem, READY));
        } else if (time > sim_time) {
            fclose(file);
            return true;
        }
    }
    // if reached EOF and no nodes added -> no processes remaining
    fclose(file);
    return false;
}

// remove process and node from linked list
void finish_process(Node **node, Process **p) {
    delete_node(node, *p);
    free(*p);
    *p = NULL;
}

void increment_sim_time(Process **p, int *sim_time, int quantum) {
    *sim_time += quantum;
    // run the process
    if (*p != NULL) {
        // decrement remaining time
        if ((*p)->rtime > quantum) {
            (*p)->rtime -= quantum;
        } else {
            // if done using CPU, remaining time is 0
            (*p)->rtime = 0;
        }
    }
}