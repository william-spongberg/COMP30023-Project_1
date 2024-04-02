#include "read.h"
#include <getopt.h>
#include <unistd.h>

void parse_command_line(int argc, char *argv[], char **filename,
                        mem_strategy *strategy, int *quantum) {
    int opt;
    while ((opt = getopt(argc, argv, "f:m:q:")) != -1) {
        switch (opt) {
        case 'f':
            // if breaks use strcpy()
            *filename = optarg;
            break;
        case 'm':
            *strategy = set_memory_strategy(optarg);
            break;
        case 'q':
            *quantum = atoi(optarg);
            break;
        default:
            fprintf(stderr,
                    "Usage: allocate -f <filename> -m (infinite | first-fit | "
                    "paged | virtual) -q (1 | 2 | 3)\n");
            exit(EXIT_FAILURE);
        }
    }

    // check if all arguments are provided
    if (filename == NULL || quantum == 0) {
        fprintf(stderr,
                "Usage: allocate -f <filename> -m (infinite | first-fit | "
                "paged | virtual) -q (1 | 2 | 3)\n");
        exit(EXIT_FAILURE);
    }
}

mem_strategy set_memory_strategy(char *mem_strategy) {
    if (strcmp(mem_strategy, "infinite") == 0) {
        return INFINITE;
    } else if (strcmp(mem_strategy, "first-fit") == 0) {
        return FIRST_FIT;
    } else if (strcmp(mem_strategy, "paged") == 0) {
        return PAGED;
    } else if (strcmp(mem_strategy, "virtual") == 0) {
        return VIRTUAL;
    }
    fprintf(stderr, "Invalid memory strategy\n");
    exit(EXIT_FAILURE);
}

int set_quantum(char *quantum) {
    int q = atoi(quantum);
    if (q == 1 || q == 2 || q == 3) {
        return q;
    }
    fprintf(stderr, "Invalid quantum\n");
    exit(EXIT_FAILURE);
}

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