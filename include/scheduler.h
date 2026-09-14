#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

/* Initialize the scheduler */
void scheduler_init(void);

/* Select and switch to the next READY process */
void schedule(void);

/* Called on every timer tick */
uint32_t scheduler_tick(uint32_t current_esp);
#endif
