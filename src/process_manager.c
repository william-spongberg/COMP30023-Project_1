#include "process_manager.h"

void run_simulation(char *filename, mem_strategy strategy, int quantum) {
    Node *queue = NULL;
    Process *p = NULL;
    p_state curr_state = READY;
    void *mem = create_memory(strategy);
    int sim_time = 0;
    stats_t stats = new_stats();

    // while processes are being loaded or there are still processes to run
    while (load_processes(&queue, filename, sim_time, quantum) ||
           (queue != NULL)) {

        // if process has completed its execution, delete it
        if (process_completed(p) && queue != NULL) {
            stats.num_processes++;
            p->state = FINISHED;
            curr_state = p->state;
            print_finished_process(p, sim_time, list_length(queue) - 1);
            finish_process(&queue, &p, &mem, strategy, sim_time, &stats);
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

        if (load_processes(&queue, filename, sim_time, quantum) ||
            (queue != NULL))
            // increment simulation arrival_time
            increment_sim_time(&p, &sim_time, quantum);
    }

    // calculate statistics
    int avg_turnaround_time = round((double)stats.total_turnaround_time / stats.num_processes);
    float avg_time_overhead = stats.total_time_overhead / stats.num_processes;
    printf("Turnaround time %d\n", avg_turnaround_time);
    printf("Time overhead %.2f %.2f\n", stats.max_time_overhead,
           avg_time_overhead);
    printf("Makespan %d\n", sim_time);

    // free memory
    destroy_memory(mem, strategy);
}

bool process_completed(Process *p) {
    if (p == NULL) {
        return false;
    }
    return p->rtime == 0;
}

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
    default:
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
}

void run_process(Process **p, void *mem, p_state *curr_state,
                 mem_strategy strategy, int sim_time) {
    // set to running
    (*p)->state = RUNNING;

    // update memory usage
    if (strategy == PAGED) {
        for (int i = 0; i < (*p)->mem / PAGE_SIZE; i++) {
            page_used(&((paged_memory_t *)mem)->lru, &(*p)->pages[i]);
        }
    }

    // if switched states, print state
    if (*curr_state != (*p)->state) {
        *curr_state = (*p)->state;
        print_process(*p, mem, strategy, sim_time);
    }
}

void finish_process(Node **node, Process **p, void **mem, mem_strategy strategy,
                    int sim_time, stats_t *stats) {
    // update stats
    int turnaround_time = sim_time - (*p)->arrival_time;
    float time_overhead = (float)turnaround_time / (float)(*p)->service_time;
    (*stats).total_turnaround_time += turnaround_time;
    (*stats).total_time_overhead += time_overhead;
    if (time_overhead > (*stats).max_time_overhead) {
        (*stats).max_time_overhead = time_overhead;
    }

    // Print eviction
    print_evicted_frames(p, (paged_memory_t *)*mem, sim_time);

    // delete node from queue and process from memory
    delete_node(node, *p);
    free_memory(mem, p, strategy, (*p)->addr, (*p)->mem);
    free(*p);
    *p = NULL;

    // Print eviction
}

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

stats_t new_stats() {
    stats_t stats;
    stats.num_processes = 0;
    stats.total_turnaround_time = 0;
    stats.total_time_overhead = 0;
    stats.max_time_overhead = 0;
    return stats;
}