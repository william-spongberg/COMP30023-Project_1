#include "process_manager.h"

void run_simulation(char *filename, mem_strategy strategy, int quantum) {
    Node *queue = NULL;
    Process *p = NULL;
    int sim_time = 0;
    void *mem = create_memory(strategy);
    p_state curr_state = READY;

    // while processes are being loaded or there are still processes to run
    while (load_processes(&queue, filename, sim_time, quantum) ||
           (queue != NULL)) {

        // if process has completed its execution, delete it
        if (process_completed(p) && queue != NULL) {
            p->state = FINISHED;
            curr_state = p->state;
            print_finished_process(p, sim_time, list_length(queue) - 1);
            finish_process(&queue, &p, &mem, strategy);
        }

        // update state of current process, state of newly allocated process
        round_robin_scheduling(&queue, &p, &curr_state);

        // check if p exists
        if (p != NULL) {
            // allocate memory if not already allocated
            if (!(attempt_allocation(&p, &mem, strategy))) {
                continue;
            }
            // run the process
            run_process(&p, mem, &curr_state, strategy, sim_time);
        }
        // increment simulation time
        increment_sim_time(&p, &sim_time, quantum);
    }
    // free memory
    destroy_memory(mem, strategy);
}

bool process_completed(Process *p) {
    if (p == NULL) {
        return false;
    }
    return p->rtime == 0;
}

void print_process(Process *p, void *mem, mem_strategy strategy,
                   int sim_time) {
    switch(strategy) {
        case INFINITE: 
            print_running_process(p, sim_time);
            break;
        
        // FIXME: Why not use if else branch since FIRST_FIT and PAGED uses the same function?
        case FIRST_FIT:
            print_memory_process(p, mem, sim_time);
            break;
        case PAGED:
            print_paged_process(p, mem, sim_time);
            break;
        default:
            fprintf(stderr, "Invalid memory strategy\n");
            exit(EXIT_FAILURE);
    }
}

void run_process(Process **p, void *mem, p_state *curr_state,
                 mem_strategy strategy, int sim_time) {
    (*p)->state = RUNNING;
    if (strategy == PAGED) {
        for (int i = 0; i < (*p)->mem / PAGE_SIZE; i++) {
            page_used(&((paged_memory_t *)mem)->lru, &(*p)->pages[i]);
        }
    }
    if (*curr_state != (*p)->state) {
        *curr_state = (*p)->state;
        print_process(*p, mem, strategy, sim_time);
    }
}

// remove process and node from linked list
void finish_process(Node **node, Process **p, void **mem,
                    mem_strategy strategy) {
    delete_node(node, *p);
    free_memory(mem, p, strategy, (*p)->addr, (*p)->mem);
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