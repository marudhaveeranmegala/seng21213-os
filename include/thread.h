#ifndef THREAD_H
#define THREAD_H

#include "types.h"

/*
 * SENG21213 - Stage 2
 * Kernel Thread Support
 */

#define MAX_THREADS 32

typedef enum {
    THREAD_UNUSED = 0,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED
} thread_state_t;

typedef struct thread {
    int tid;
    char name[32];

    thread_state_t state;

    /* Thread function and argument */
    void (*entry)(void *);
    void *arg;

    /* Saved stack pointer */
    uint32_t esp;

    /* Private kernel stack - 4 KB */
    uint32_t stack[1024];

} thread_t;

/* Initialise the thread system */
void thread_init(void);

/* Create a new kernel thread */
int thread_create(void (*entry)(void *), void *arg, const char *name);

/* Display all threads */
void thread_list(void);

/* Get a thread from the thread table */
thread_t *thread_get(int index);

/* Get number of active threads */
int thread_count(void);

#endif
