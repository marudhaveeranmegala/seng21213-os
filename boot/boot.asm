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

    ; GRUB provides Multiboot information:
    ; EAX = Multiboot magic number
    ; EBX = address of Multiboot information structure
    mov [multiboot_magic], eax
    mov [multiboot_info], ebx

     mov esp, stack_top

    ; Write "BOOT" directly to VGA memory.
    ; This happens before calling the C kernel.
    mov word [0xB8000], 0x0F42
    mov word [0xB8002], 0x0F4F
    mov word [0xB8004], 0x0F4F
    mov word [0xB8006], 0x0F54

    push dword [multiboot_info]
    push dword [multiboot_magic]
    call kernel_main
    add esp, 8




  .hang:
      hlt
      jmp .hang







section .bss
align 16

multiboot_magic:
    resd 1

multiboot_info:
    resd 1


stack_bottom:
    resb 16384
stack_top:
