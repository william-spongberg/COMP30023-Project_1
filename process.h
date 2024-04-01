#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// Define enum for process states
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} process_state_t;

// Define process structure
typedef struct process {
    int arrival_time;
    char *process_name;
    int burst_time;
    int memory_size;
    int process_state;
} process_t;

// Define process functions
process_t *create_process(int arraival_time, char *process_name, int burst_time, int memory_size);
void update_process_burst_time(process_t *process, int burst_time);
void update_process_state(process_t *process, process_state_t state);
void terminate_process(process_t *process);

