#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "types.h"

typedef struct {
    int value;
} semaphore_t;

void semaphore_init(semaphore_t *sem, int value);
int semaphore_wait(semaphore_t *sem);
int semaphore_signal(semaphore_t *sem);

#endif
