#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"

typedef struct {
    int locked;
    int owner;
} mutex_t;

void mutex_init(mutex_t *mutex);
int mutex_lock(mutex_t *mutex, int tid);
int mutex_unlock(mutex_t *mutex, int tid);

#endif
