#include <process.h>
#include <scheduler.h>
#include <thread.h>

/*
 * Current process index.
 * -1 means that no process has been scheduled yet.
 */
static int current_process = -1;
static int current_thread = -1;

/*
 * Initialize the scheduler.
 */
void scheduler_init(void)
{
    current_process = -1;
    current_thread = -1;
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

    /*
     * No user processes exist.
     * Schedule kernel threads instead.
     */
    int thread_count_value = thread_count();

    if (thread_count_value <= 0) {
        return current_esp;
    }

    /*
     * Save the current thread's stack pointer.
     */
    if (current_thread >= 0) {
        thread_t *current_thread_ptr =
            thread_get(current_thread);

        if (current_thread_ptr != 0) {
            current_thread_ptr->esp = current_esp;

            if (current_thread_ptr->state == THREAD_RUNNING) {
                current_thread_ptr->state = THREAD_READY;
            }
        }
    }

    /*
     * Find the next READY thread.
     */
    for (int i = 1; i <= thread_count_value; i++) {

        int next_thread =
            (current_thread + i) % thread_count_value;

        thread_t *next_thread_ptr =
            thread_get(next_thread);

        if (next_thread_ptr != 0 &&
            next_thread_ptr->state == THREAD_READY) {

            current_thread = next_thread;
            next_thread_ptr->state = THREAD_RUNNING;

            return next_thread_ptr->esp;
        }
    }

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
