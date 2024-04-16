#include "process_manager.h"

// run the simulation
void run_simulation(char *filename, mem_strategy strategy, int quantum) {
    Node *queue = NULL;
    Process *p = NULL;
    void *mem = create_memory(strategy);
    pqueue_t *lru_queue = create_pqueue();
    p_state curr_state = READY;

    stats_t stats = new_stats();
    int sim_time = 0;
    int num_lines = 0;

    // while processes to read from file or processes in queue
    while (
        processes_remaining(&queue, filename, &num_lines, sim_time, quantum)) {

        // if process has completed its execution, delete it
        if (is_process_finished(p) && queue != NULL) {
            finish_process(&queue, &p, &mem, strategy, lru_queue, sim_time,
                           &curr_state, &stats);
        }

        // update state of current process, state of newly allocated process
        round_robin_scheduling(&queue, &p, &curr_state);

        // check if p exists
        if (p != NULL) {
            // allocate memory if not already allocated
            if (!(attempt_alloc(&p, &mem, strategy, lru_queue, sim_time))) {
                continue;
            }
            // run the process
            run_process(&p, mem, &curr_state, strategy, sim_time, lru_queue);
        }

        // if processes still remaining, increment simulation time
        if (processes_remaining(&queue, filename, &num_lines, sim_time,
                                quantum)) {
            increment_sim_time(&p, &sim_time, quantum);
        }
    }

    // calculate statistics
    print_stats(stats, sim_time);

    // free memory
    free_memory_structs(&mem, &lru_queue);
}

// check if processes are remaining
bool processes_remaining(Node **queue, char *filename, int *num_lines,
                         int sim_time, int quantum) {
    return (load_processes_from_file(queue, filename, num_lines, sim_time,
                                     quantum) != -1) ||
           (*queue != NULL);
}

// increment simulation time + decrement process time
void increment_sim_time(Process **p, int *sim_time, int quantum) {
    *sim_time += quantum;
    // run the process
    if (*p != NULL) {
        // decrement remaining arrival_time
        if ((*p)->rtime > quantum) {
            (*p)->rtime -= quantum;
        } else {
            // if done using CPU, remaining arrival_time is 0
            (*p)->rtime = 0;
        }
    }
}

// run process execution
void run_process(Process **p, void *mem, p_state *curr_state,
                 mem_strategy strategy, int sim_time, pqueue_t *lru_queue) {
    // set to running
    (*p)->state = RUNNING;

    // Record last execution time
    (*p)->last_exec = sim_time;

    if (strategy == PAGED || strategy == VIRTUAL) {
        // If process is not in queue enqueue, else heapify the queue
        if (!in_queue(lru_queue, *p)) {
            enqueue(lru_queue, *p);
        } else {
            heapify(lru_queue);
        }
    }

    // if switched states, print state
    if (*curr_state != (*p)->state) {
        *curr_state = (*p)->state;
        print_process(*p, mem, strategy, sim_time);
    }
}

// finish process execution
void finish_process(Node **node, Process **p, void **mem, mem_strategy strategy,
                    pqueue_t *lru_queue, int sim_time, p_state *curr_state,
                    stats_t *stats) {
    // update stats
    (*stats).num_processes += 1;
    int turnaround_time = sim_time - (*p)->arrival_time;
    float time_overhead = (float)turnaround_time / (float)(*p)->service_time;

    (*stats).total_turnaround_time += turnaround_time;
    (*stats).total_time_overhead += time_overhead;
    if (time_overhead > (*stats).max_time_overhead) {
        (*stats).max_time_overhead = time_overhead;
    }

    (*p)->state = FINISHED;
    (*curr_state) = (*p)->state;

    // delete node from queue
    delete_node(node, *p);

    // print evicted frames + finished process
    if (strategy == PAGED || strategy == VIRTUAL) {
        delete_process(lru_queue, *p);
        print_evicted_frames(*p, sim_time, get_page_count(*p));
    }
    print_finished_process(*p, sim_time, list_length(*node));

    // free memory
    free_process_memory(mem, p, strategy, (*p)->addr, (*p)->mem);
    free_process(p);
}

// check if process has finished executing
bool is_process_finished(Process *p) {
    if (p == NULL) {
        return false;
    }
    return p->rtime == 0;
}

// print process info based on memory strategy
void print_process(Process *p, void *mem, mem_strategy strategy, int sim_time) {
    switch (strategy) {
    case INFINITE:
        print_running_process(p, sim_time);
        break;
    case FIRST_FIT:
        print_memory_process(p, mem, sim_time);
        break;
    case PAGED:
        print_paged_process(p, mem, sim_time);
        break;
    case VIRTUAL:
        print_paged_process(p, mem, sim_time);
        break;
    default:
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
}

// create new statistics struct
stats_t new_stats() {
    stats_t stats;
    stats.num_processes = 0;
    stats.total_turnaround_time = 0;
    stats.total_time_overhead = 0;
    stats.max_time_overhead = 0;
    return stats;
}

// print statistics
void print_stats(stats_t stats, int sim_time) {
    int avg_turnaround_time =
        ceil((double)stats.total_turnaround_time / stats.num_processes);
    float avg_time_overhead = stats.total_time_overhead / stats.num_processes;
    printf("Turnaround time %d\n", avg_turnaround_time);
    printf("Time overhead %.2f %.2f\n", stats.max_time_overhead,
           avg_time_overhead);
    printf("Makespan %d\n", sim_time);
}

// free memory structs
void free_memory_structs(void **mem, pqueue_t **lru_queue) {
    free(*mem);
    *mem = NULL;
    free_pqueue(*lru_queue);
}