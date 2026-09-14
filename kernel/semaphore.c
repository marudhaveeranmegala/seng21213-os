#include "semaphore.h"

void semaphore_init(semaphore_t *sem, int value)
{
    if (sem == 0) {
        return;
    }

    sem->value = value;
}

int semaphore_wait(semaphore_t *sem)
{
    if (sem == 0) {
        return -1;
    }

    if (sem->value <= 0) {
        return -1;
    }

    sem->value--;

    return 0;
}

int semaphore_signal(semaphore_t *sem)
{
    if (sem == 0) {
        return -1;
    }

    sem->value++;

    return 0;
}
