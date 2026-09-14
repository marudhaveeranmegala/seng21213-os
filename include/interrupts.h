#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"

/* IDT entry for 32-bit protected mode */
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

/* IDT pointer used by the lidt instruction */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/* Initialize the Interrupt Descriptor Table */
void idt_init(void);

/* Initialize the 8259 Programmable Interrupt Controller */
void pic_init(void);

/* Initialize the Programmable Interval Timer */
void pit_init(void);

#endif
