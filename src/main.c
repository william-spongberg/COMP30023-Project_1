#include "process_manager.h"
#include <getopt.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *filename = NULL;
    mem_strategy strategy;
    int quantum = 0;

    // parse command line arguments
    parse_command_line(argc, argv, &filename, &strategy, &quantum);

    // run simulation
    run_simulation(filename, strategy, quantum);

    return 0;
}