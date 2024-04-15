#ifndef READ_H
#define READ_H

#include "queue.h"
#include <getopt.h>
#include <unistd.h>

typedef enum { INFINITE, FIRST_FIT, PAGED, VIRTUAL } mem_strategy;

// method signatures
void parse_command_line(int argc, char *argv[], char **filename,
                        mem_strategy *strategy, int *quantum);
mem_strategy set_memory_strategy(char *mem_strategy);
int set_quantum(char *quantum);
FILE *open_file(char *filename);
int load_processes_from_file(Node **queue, char *filename, int *num_lines,
                             int sim_time, int quantum);

#endif