#include <process.h>
#include <scheduler.h>

/*
 * Current process index.
 * -1 means that no process has been scheduled yet.
 */
static int current_process = -1;

/*
 * Initialize the scheduler.
 */
void scheduler_init(void)
{
    current_process = -1;
}

/*
 * Select the next READY process using Round-Robin scheduling.
 *
 * current_esp is the stack pointer saved by the IRQ handler.
 *
 * Returns the ESP value that the assembly IRQ handler should restore.
 */
uint32_t scheduler_tick(uint32_t current_esp)
{
    int count = process_count();

    if (count <= 0) {
        return current_esp;
    }

    /*
     * Save the current process's stack pointer.
     */
    if (current_process >= 0) {
        process_t *current = process_get(current_process);

        if (current != 0) {
            current->esp = current_esp;

            if (current->state == PROCESS_RUNNING) {
                current->state = PROCESS_READY;
            }
        }
    }

    /*
     * Find the next READY process.
     * This implements Round-Robin scheduling.
     */
    for (int i = 1; i <= count; i++) {

        int next = (current_process + i) % count;

        process_t *next_process = process_get(next);

        if (next_process != 0 &&
            next_process->state == PROCESS_READY) {

            current_process = next;
            next_process->state = PROCESS_RUNNING;

            return next_process->esp;
        }
    }

    /*
     * No READY process was found.
     * Continue with the current interrupt stack.
     */
    return current_esp;
}

/*
 * Compatibility function.
 * The timer interrupt uses scheduler_tick().
 */
void schedule(void)
{
    /* Scheduling is performed by scheduler_tick(). */
}
