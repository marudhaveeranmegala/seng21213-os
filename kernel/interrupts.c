#include "interrupts.h"
#include "io.h"
#include "vga.h"

#define IDT_ENTRIES 256

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA    0x21
#define PIC_SLAVE_COMMAND  0xA0
#define PIC_SLAVE_DATA     0xA1

#define PIC_EOI            0x20

#define PIT_COMMAND        0x43
#define PIT_CHANNEL0       0x40
#define PIT_FREQUENCY      1193182
#define PIT_HZ             100

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_pointer;

/* IRQ0 handler implemented in assembly */
extern void irq0_handler(void);

/* Set one IDT entry */
static void idt_set_gate(
    int number,
    uint32_t handler,
    uint16_t selector,
    uint8_t flags
)
{
    idt[number].offset_low =
        (uint16_t)(handler & 0xFFFF);

    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].type_attr = flags;

    idt[number].offset_high =
        (uint16_t)((handler >> 16) & 0xFFFF);
}

/* Load the IDT */
static void idt_load(void)
{
    __asm__ volatile (
        "lidtl (%0)"
        :
        : "r"(&idt_pointer)
    );
}

void idt_init(void)
{
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    idt_set_gate(
        32,
        (uint32_t)irq0_handler,
        0x10,
        0x8E
    );

    idt_pointer.limit =
        sizeof(idt) - 1;

    idt_pointer.base =
        (uint32_t)&idt;

    idt_load();
}

/* Remap the 8259 PIC */

   static void io_wait(void)
{
    outb(0x80, 0);
}

void pic_init(void)
{
    /* Start PIC initialization sequence. */
    outb(PIC_MASTER_COMMAND, 0x11);
    io_wait();

    outb(PIC_SLAVE_COMMAND, 0x11);
    io_wait();

    /* Remap master PIC IRQs 0-7 to vectors 0x20-0x27. */
    outb(PIC_MASTER_DATA, 0x20);
    io_wait();

    /* Remap slave PIC IRQs 8-15 to vectors 0x28-0x2F. */
    outb(PIC_SLAVE_DATA, 0x28);
    io_wait();

    /* Tell master PIC that slave is connected to IRQ2. */
    outb(PIC_MASTER_DATA, 0x04);
    io_wait();

    /* Tell slave PIC its cascade identity. */
    outb(PIC_SLAVE_DATA, 0x02);
    io_wait();

    /* Set 8086/88 mode. */
    outb(PIC_MASTER_DATA, 0x01);
    io_wait();

    outb(PIC_SLAVE_DATA, 0x01);
    io_wait();

    /*
     * Enable only IRQ0.
     * Mask all other master and slave IRQ lines.
     */
    outb(PIC_MASTER_DATA, 0xFE);
    io_wait();

    outb(PIC_SLAVE_DATA, 0xFF);
    io_wait();

    vga_puts("PIC initialized\n");

}


/* Configure PIT for 100 Hz */
void pit_init(void)
{
    uint16_t divisor =
        (uint16_t)(PIT_FREQUENCY / PIT_HZ);

    /*
     * Channel 0
     * Access mode: low byte then high byte
     * Mode 3: square wave generator
     */
    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0,
         (uint8_t)(divisor & 0xFF));

    outb(PIT_CHANNEL0,
         (uint8_t)((divisor >> 8) & 0xFF));
}
