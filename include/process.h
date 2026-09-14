#ifndef PROCESS_H
#define PROCESS_H

#include <types.h>

typedef enum {
    PROCESS_TERMINATED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED
} process_state_t;

void process_init(void);
int create_process(void (*entry_point)(void), const char *name);
void process_list(void);
void schedule(void);

typedef struct process {
    int pid;
    char name[32];
    process_state_t state;
    void (*entry_point)(void);  // Process entry function
    uint32_t esp;               // Saved stack pointer
    uint32_t stack[1024];       // 4KB stack
} process_t;

/* Scheduler access to the process table */
process_t *process_get(int index);
int process_count(void);

#endif
