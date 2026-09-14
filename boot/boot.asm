[BITS 32]
global _start
extern kernel_main

MULTIBOOT_ALIGN    equ 1 << 0
MULTIBOOT_MEMINFO  equ 1 << 1
MULTIBOOT_FLAGS    equ MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO
MULTIBOOT_MAGIC    equ 0x1BADB002
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
    align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

    section .text
 _start:
    cli

    mov esp, stack_top

    ; Write "BOOT" directly to VGA memory.
    ; This happens before calling the C kernel.
    mov word [0xB8000], 0x0F42
    mov word [0xB8002], 0x0F4F
    mov word [0xB8004], 0x0F4F
    mov word [0xB8006], 0x0F54

   call kernel_main


  .hang:
      hlt
      jmp .hang







section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
