# SENG21213-OS — Stage 1: Process Management

> **Course**: SENG 21213 – Computer Architecture & Operating Systems
> **Year**: 2nd Year, Software Engineering
> **Assignment**: Build your own x86 Operating System

---

## What Is This?

This project is a small educational x86 operating system developed for the
SENG 21213 Computer Architecture & Operating Systems assignment.

Stage 1 implements basic process management and preemptive Round-Robin
scheduling using the Programmable Interval Timer (PIT) and IRQ0.

---

## Stage 1 Features

The following Stage 1 features are implemented:

- Process Control Block (PCB)
- Process ID (PID)
- Process states
- 4 KB stack for each process
- Process creation using `create_process()`
- Process listing using `ps`
- 100 Hz PIT timer
- IRQ0 timer interrupt
- PIC initialization
- IDT entry for IRQ0
- Round-Robin scheduling
- Process context switching
- Two concurrent kernel processes

The two demonstration processes are:

- `ProcessA` - prints `A`
- `ProcessB` - prints `B`

When scheduling is working correctly, QEMU displays output similar to:

```text
ABABABABABABABABABABABAB...
