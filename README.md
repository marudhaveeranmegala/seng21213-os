# SENG21213-OS — x86 Operating System

> **Course**: SENG 21213 – Computer Architecture & Operating Systems
> **Year**: 2nd Year, Software Engineering
> **Assignment**: Build your own x86 Operating System

---

## What Is This?

This project is a small educational x86 operating system developed for the
SENG 21213 Computer Architecture & Operating Systems assignment.

The operating system is developed from scratch using C and x86 Assembly and
runs in QEMU in 32-bit protected mode.

---

## Implemented Stages

### Stage 0 — Boot, VGA, Keyboard and Shell

* x86 boot process
* VGA text-mode display
* Keyboard input
* Basic kernel shell
* `help`
* `clear`
* `echo`
* `about`
* `version`
* `colour`
* `halt`

### Stage 1 — Process Management and Scheduling

* Process Control Block (PCB)
* Process IDs (PID)
* Process states
* 4 KB process stacks
* Process creation
* Process listing using `ps`
* 100 Hz PIT timer
* IRQ0 timer interrupt
* PIC initialization
* IDT entry for IRQ0
* Round-Robin scheduling
* Process context switching
* Two concurrent kernel processes

### Stage 2 — Threads and Synchronization

* Kernel thread support
* Thread creation
* Thread management
* Mutex synchronization
* Semaphore synchronization
* Basic thread scheduling and synchronization primitives

### Stage 3 — Physical Memory Management

* Physical Memory Manager (PMM)
* Page/block allocation and release
* Memory bitmap management
* Physical memory tracking
* `free` command for memory information
* Kernel memory management support

### Stage 4 — RAM Disk File System

* 1 MB RAM disk
* 4 KB filesystem blocks
* Filesystem superblock
* Block bitmap
* Inode bitmap
* Inode table
* Flat directory
* Direct block pointers
* File creation
* File opening and closing
* File reading and writing
* File deletion
* Shell commands:

  * `ls`
  * `touch`
  * `cat`
  * `write`
  * `rm`

The Stage 4 filesystem was tested by creating, writing, reading and deleting
at least five files.

---

## Stage 4 File System Layout

The RAM disk uses the following layout:

| Block | Purpose      |
| ----- | ------------ |
| 0     | Superblock   |
| 1     | Directory    |
| 2     | Block bitmap |
| 3     | Inode bitmap |
| 4     | Inode table  |
| 5+    | Data blocks  |

The filesystem supports up to 64 inodes and uses eight direct block pointers
per inode.

---

## Build and Run

Build the operating system:

```bash
make clean
make
```

Run it in QEMU:

```bash
make run
```

---

## Stage 4 Shell Examples

Create a file:

```text
touch file1
```

List files:

```text
ls
```

Write data:

```text
write file1 Hello
```

Read a file:

```text
cat file1
```

Delete a file:

```text
rm file1
```

---

## Git Tags

The completed assignment stages are tagged as follows:

```text
v0.1-stage0
v0.2-stage1
v0.3-stage2
v0.4-stage3
v0.5-stage4
```

The final Stage 4 submission is:

```text
v0.5-stage4
```

---

## Development Environment

* x86 32-bit protected mode
* C
* x86 Assembly
* NASM
* GCC
* GNU Make
* QEMU
* GDB
* WSL2 Ubuntu

---

## Educational Purpose

This operating system is developed as an educational project to understand
computer architecture, operating system fundamentals, process management,
scheduling, synchronization, memory management and file systems.
