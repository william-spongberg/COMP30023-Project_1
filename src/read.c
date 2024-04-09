#include "read.h"

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

FILE *open_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    return file;
}

int load_processes(Node **queue, char *filename, int num_lines, int sim_time, int quantum) {
    if (num_lines == -1) {
        return -1;
    }

    // open file
    FILE *file = open_file(filename);

    // format: arrival-time process-name remaining-time memory
    Process *p_temp;
    char name[8];
    int arrival_time, rtime;
    int mem = 0;

    // read up to current line
    for (int i = 0; i < num_lines; i++) {
        fscanf(file, "%d %s %d %d", &arrival_time, name, &rtime, &mem);
    }

    // read file
    while (fscanf(file, "%d %s %d %d", &arrival_time, name, &rtime, &mem) != EOF) {
        // if arrived since last cycle, set to READY and add to linked list
        if ((arrival_time >= (sim_time - quantum)) &&
            (arrival_time <= sim_time)) {
            //printf("inserting %s\n", name);
            p_temp = create_process(arrival_time, name, rtime, mem, READY);
            insert_node(queue, &p_temp);
            num_lines++;
        } else if (arrival_time > sim_time) {
            fclose(file);
            file = NULL;
            return num_lines;
        }
    }

    // if reached EOF and no nodes added -> no processes remaining
    fclose(file);
    file = NULL;
    return -1;
}