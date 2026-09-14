#include "thread.h"
#include "vga.h"

/*
 * SENG21213 - Stage 2
 * Kernel Thread Support
 *
 * Kernel threads share the kernel address space but have
 * their own stacks and execution functions.
 */

static thread_t thread_table[MAX_THREADS];
static int next_tid = 1;

/*
 * Initialise the thread table.
 */
void thread_init(void)
{
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_table[i].tid = 0;
        thread_table[i].state = THREAD_UNUSED;
        thread_table[i].entry = 0;
        thread_table[i].arg = 0;
        thread_table[i].esp = 0;
    }

    next_tid = 1;
}

/*
 * Create a new kernel thread.
 *
 * The thread starts in THREAD_READY state.
 * A stack frame is prepared so that the scheduler can
 * eventually start executing the thread function.
 */
int thread_create(void (*entry)(void *), void *arg, const char *name)
{
    if (entry == 0) {
        return -1;
    }

    for (int i = 0; i < MAX_THREADS; i++) {

        if (thread_table[i].state == THREAD_UNUSED ||
            thread_table[i].state == THREAD_TERMINATED) {

            thread_t *t = &thread_table[i];

            t->tid = next_tid++;
            t->state = THREAD_READY;
            t->entry = entry;
            t->arg = arg;

            /*
             * Copy the thread name.
             */
            int j = 0;

            if (name != 0) {
                while (name[j] != '\0' && j < 31) {
                    t->name[j] = name[j];
                    j++;
                }
            }

            t->name[j] = '\0';

            /*
             * Prepare the initial stack.
             *
             * The stack grows downward.
             */
            uint32_t *sp = &t->stack[1024];

            /*
             * Initial EFLAGS.
             */
            *(--sp) = 0x202;

            /*
             * Code segment.
             */
            *(--sp) = 0x10;

            /*
             * Initial instruction pointer.
             */
            *(--sp) = (uint32_t)entry;

            /*
             * Values corresponding to registers restored by POPAD.
             */
            *(--sp) = 0; /* EAX */
            *(--sp) = 0; /* ECX */
            *(--sp) = 0; /* EDX */
            *(--sp) = 0; /* EBX */
            *(--sp) = 0; /* Original ESP */
            *(--sp) = 0; /* EBP */
            *(--sp) = 0; /* ESI */
            *(--sp) = 0; /* EDI */

            t->esp = (uint32_t)sp;

            return t->tid;
        }
    }

    return -1;
}

/*
 * Return a thread from the thread table.
 */
thread_t *thread_get(int index)
{
    if (index < 0 || index >= MAX_THREADS) {
        return 0;
    }

    return &thread_table[index];
}

/*
 * Count active threads.
 */
int thread_count(void)
{
    int count = 0;

    for (int i = 0; i < MAX_THREADS; i++) {
        if (thread_table[i].state != THREAD_UNUSED &&
            thread_table[i].state != THREAD_TERMINATED) {
            count++;
        }
    }

    return count;
}

/*
 * Display the current thread table.
 */
void thread_list(void)
{
    vga_puts("\n  TID | STATE     | NAME\n");
    vga_puts("  -------------------------\n");

    for (int i = 0; i < MAX_THREADS; i++) {

        thread_t *t = &thread_table[i];

        if (t->state == THREAD_UNUSED ||
            t->state == THREAD_TERMINATED) {
            continue;
        }

        vga_puts("  ");

        if (t->tid < 10) {
            vga_puts(" ");
        }

        /*
         * This version keeps output simple because the kernel
         * does not yet have a full integer-to-string library.
         */
        if (t->tid == 1) {
            vga_puts("1");
        } else if (t->tid == 2) {
            vga_puts("2");
        } else if (t->tid == 3) {
            vga_puts("3");
        } else {
            vga_puts("?");
        }

        vga_puts("   | ");

        switch (t->state) {
            case THREAD_READY:
                vga_puts("READY     | ");
                break;

            case THREAD_RUNNING:
                vga_puts("RUNNING   | ");
                break;

            case THREAD_BLOCKED:
                vga_puts("BLOCKED   | ");
                break;

            default:
                vga_puts("UNKNOWN   | ");
                break;
        }

        vga_puts(t->name);
        vga_puts("\n");
    }
}
