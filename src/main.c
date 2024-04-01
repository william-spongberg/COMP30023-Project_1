#include "process_manager.h"
#include <getopt.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *memory_strategy = NULL;
    int quantum = 0;
    int opt;

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

    // simulation
    run_simulation(filename, memory_strategy, quantum);

    return 0;
}