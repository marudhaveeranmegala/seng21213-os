[BITS 32]

global irq0_handler

extern scheduler_tick

section .text

irq0_handler:
    ; Save all general-purpose registers
    pushad

    ; Pass the current process stack pointer to scheduler_tick()
    ; After pushad, ESP points to the saved register frame.
    push esp
    call scheduler_tick
    add esp, 4

    ; scheduler_tick() returns the ESP of the process
    ; that should run next.
    mov esp, eax

    ; Send End Of Interrupt (EOI) to the master PIC
    mov al, 0x20
    out 0x20, al

    ; Restore the selected process's registers
    popad

    ; Return from the hardware interrupt
    iretd
