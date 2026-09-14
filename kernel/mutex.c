#include "mutex.h"

void mutex_init(mutex_t *mutex)
{
    if (mutex == 0) {
        return;
    }

    mutex->locked = 0;
    mutex->owner = -1;
}

int mutex_lock(mutex_t *mutex, int tid)
{
    if (mutex == 0) {
        return -1;
    }

    if (mutex->locked == 0) {
        mutex->locked = 1;
        mutex->owner = tid;
        return 0;
    }

    return -1;
}

int mutex_unlock(mutex_t *mutex, int tid)
{
    if (mutex == 0) {
        return -1;
    }

    if (mutex->locked == 1 && mutex->owner == tid) {
        mutex->locked = 0;
        mutex->owner = -1;
        return 0;
    }

    return -1;
}
