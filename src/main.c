#include "round_robin.h"
#include <getopt.h>
#include <unistd.h>

Process **load_processes(char *filename, int *n);
void set_memory_strategy(char *memory_strategy);

int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *memory_strategy = NULL;
    int quantum = 0;
    int opt;
    int n = 0;

    // parse command line arguments
    while ((opt = getopt(argc, argv, "f:m:q:")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'm':
            memory_strategy = optarg;
            break;
        case 'q':
            quantum = atoi(optarg);
            break;
        default:
            fprintf(
                stderr,
                "Usage: %s -f <filename> -m <memory-strategy> -q <quantum>\n",
                argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // check if all arguments are provided
    if (filename == NULL || memory_strategy == NULL || quantum == 0) {
        fprintf(stderr, "Missing arguments\n");
        exit(EXIT_FAILURE);
    }

    // load processes and memory strategy from file
    Process **processes = load_processes(filename, &n);
    set_memory_strategy(memory_strategy);

    // do round robin scheduling
    round_robin_scheduling(processes, n, quantum);

    return 0;
}

Process **load_processes(char *filename, int *n) {
    // open file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    // unknown number of processes
    Process **processes = (Process **)malloc(sizeof(Process *));
    assert(processes != NULL);

    // format: time process-name remaining-time memory
    // e.g. 0 P1 10 100
    char name[8];
    int time, rtime;
    int mem = 0;

    // read file
    while (fscanf(file, "%d %s %d %d", &time, name, &rtime, &mem) != EOF) {
        processes =
            (Process **)realloc(processes, (*n + 1) * sizeof(Process *));
        assert(processes != NULL);
        // TODO: set to BLOCKED since haven't arrived in CPU yet?
        // initial state of READY
        processes[*n] = create_process(time, name, rtime, mem, READY);
        *n += 1;
    }
    fclose(file);

    return processes;
}

// TODO: set memory strategies
void set_memory_strategy(char *memory_strategy) {
    if (strcmp(memory_strategy, "infinite") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "first-fit") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "paged") == 0) {
        // set strategy
    } else if (strcmp(memory_strategy, "virtual") == 0) {
        // set strategy
    } else {
        fprintf(stderr, "Invalid memory strategy\n");
        exit(EXIT_FAILURE);
    }
}