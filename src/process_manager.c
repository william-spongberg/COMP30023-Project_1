#include "process_manager.h"

void run_simulation(char *filename, char *memory_strategy, int quantum) {
    Node *queue = NULL;
    Process *p = NULL;
    int sim_time = 0;
    memory_t *mem = create_memory(memory_strategy);
    p_state curr_state = READY;

    // while processes are being loaded or there are still processes to run
    while (load_processes(&queue, filename, sim_time, quantum) ||
           (queue != NULL)) {

        // if process has completed its execution, delete it
        if (process_completed(p) && queue != NULL) {
            p->state = FINISHED;
            curr_state = p->state;
            print_finished_process(p, sim_time, list_length(queue) - 1);
            finish_process(&queue, &p, &mem);
        }

        // update state of current process, state of newly allocated process
        round_robin_scheduling(&queue, &p, &curr_state);

        // check if p exists
        if (p != NULL) {
            // allocate memory if not already allocated
            // TODO: refactor into memory_manager
            if (mem != NULL) {
                if (p->addr == -1) {
                    if (mem->available >= p->mem) {
                        p->addr = allocate_memory(mem, memory_strategy, p->mem);
                    }
                    // if memory not available, continue to next iteration
                    else {
                        continue;
                    }
                }
            }
            // run the process
            run_process(&p, mem, &curr_state, memory_strategy, sim_time);
        }
        // increment simulation time
        increment_sim_time(&p, &sim_time, quantum);
    }
    // free memory
    destroy_memory(mem);
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
    Process *p_temp;
    char name[8];
    int time, rtime;
    int mem = 0;

    // read file
    while (fscanf(file, "%d %s %d %d", &time, name, &rtime, &mem) != EOF) {
        // if arrived since last cycle, set to READY and add to linked list
        if ((time >= (sim_time - quantum)) && (time <= sim_time)) {
            p_temp = create_process(time, name, rtime, mem, READY);
            insert_node(queue, &p_temp);
        } else if (time > sim_time) {
            fclose(file);
            return true;
        }
    }
    // if reached EOF and no nodes added -> no processes remaining
    fclose(file);
    return false;
}

bool process_completed(Process *p) {
    if (p == NULL) {
        return false;
    }
    return p->rtime == 0;
}

void print_process(Process *p, memory_t *mem, char *memory_strategy,
                   int sim_time) {
    if (strcmp(memory_strategy, "infinite") == 0) {
        print_running_process(p, sim_time);
    } else if (strcmp(memory_strategy, "first-fit") == 0) {
        print_memory_process(p, mem, sim_time);
    } else {
        printf("ERROR: invalid memory strategy\n");
    }
}

void run_process(Process **p, memory_t *mem, p_state *curr_state,
                 char *memory_strategy, int sim_time) {
    (*p)->state = RUNNING;
    if (*curr_state != (*p)->state) {
        *curr_state = (*p)->state;
        print_process(*p, mem, memory_strategy, sim_time);
    }
}

// remove process and node from linked list
void finish_process(Node **node, Process **p, memory_t **mem) {
    delete_node(node, *p);
    free_memory(*mem, (*p)->addr, (*p)->mem);
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