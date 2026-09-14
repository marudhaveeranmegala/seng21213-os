#include <process.h>
#include "vga.h"

#define MAX_PROCESSES 16

static process_t pcb_list[MAX_PROCESSES];
static int next_pid = 1;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pcb_list[i].state = PROCESS_TERMINATED;
        pcb_list[i].pid = 0;
        pcb_list[i].esp = 0;
    }

    next_pid = 1;
}

int create_process(void (*entry_point)(void), const char *name) {
    for (int i = 0; i < MAX_PROCESSES; i++) {

        if (pcb_list[i].state == PROCESS_TERMINATED) {

            pcb_list[i].pid = next_pid++;
            pcb_list[i].state = PROCESS_READY;
            pcb_list[i].entry_point = entry_point;

       /*
 * Build the initial stack for the process.
 *
 * The IRQ handler expects the stack in this order:
 *
 *   EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
 *   EIP, CS, EFLAGS
 *
 * POPAD restores the first eight values.
 * IRETD then restores EIP, CS and EFLAGS.
 */

/* Build the interrupt return frame first. */
     uint32_t *sp = &pcb_list[i].stack[1024];

        *(--sp) = 0x202;                    /* EFLAGS */
        *(--sp) = 0x10;                     /* CS */
        *(--sp) = (uint32_t)entry_point;    /* EIP */

/* Build the POPAD register frame. */
        *(--sp) = 0;    /* EAX */
        *(--sp) = 0;    /* ECX */
        *(--sp) = 0;    /* EDX */
        *(--sp) = 0;    /* EBX */
        *(--sp) = 0;    /* Original ESP - ignored by POPAD */
        *(--sp) = 0;    /* EBP */
        *(--sp) = 0;    /* ESI */
        *(--sp) = 0;    /* EDI */

         pcb_list[i].esp = (uint32_t)sp;



            /* Copy process name */
            int j = 0;

            while (name[j] && j < 31) {
                pcb_list[i].name[j] = name[j];
                j++;
            }

            pcb_list[i].name[j] = '\0';

            return pcb_list[i].pid;
        }
    }

    return -1;
}

void process_list(void) {
    vga_puts("PID | STATE | NAME\n");
    vga_puts("------------------\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {

        if (pcb_list[i].state != PROCESS_TERMINATED) {

            const char *state = "READY";

            if (pcb_list[i].state == PROCESS_RUNNING) {
                state = "RUNNING";
            } else if (pcb_list[i].state == PROCESS_BLOCKED) {
                state = "BLOCKED";
            }

            vga_printf("%d | %s | %s\n",
                       pcb_list[i].pid,
                       state,
                       pcb_list[i].name);
        }
    }
}
process_t *process_get(int index) {
    if (index < 0 || index >= MAX_PROCESSES) {
        return 0;
    }

    return &pcb_list[index];
}

int process_count(void)
{
    int count = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (pcb_list[i].state != PROCESS_TERMINATED) {
            count++;
        }
    }

    return count;
}
