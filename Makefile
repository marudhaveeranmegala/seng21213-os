# =========================================================================
# SENG21213-OS :: Makefile
# =========================================================================

AS      := nasm
ASFLAGS := -f elf32

ifneq (, $(shell which i686-elf-gcc 2>/dev/null))
    CC      := i686-elf-gcc
    LD      := i686-elf-ld
    CFLAGS  := -m32 -ffreestanding -fno-stack-protector -fno-pie -nostdlib \
               -Wall -Wextra -O2 -I./include
    LDFLAGS := -m elf_i386 -nostdlib
else
    CC      := gcc
    LD      := ld
    CFLAGS  := -m32 -ffreestanding -fno-stack-protector -fno-pie -nostdlib \
               -Wall -Wextra -O2 -I./include
    LDFLAGS := -m elf_i386
endif

BUILD    := build
KERN_DIR := kernel
BOOT_DIR := boot

BOOT_SRCS := $(BOOT_DIR)/boot.asm $(BOOT_DIR)/switch.asm $(BOOT_DIR)/interrupts.asm
BOOT_OBJS := $(BUILD)/boot/boot.o $(BUILD)/boot/switch.o $(BUILD)/boot/interrupts.o


KERN_SRCS := \
    $(KERN_DIR)/kernel.c \
    $(KERN_DIR)/shell.c \
    $(KERN_DIR)/vga.c \
    $(KERN_DIR)/keyboard.c \
    $(KERN_DIR)/process.c \
    $(KERN_DIR)/scheduler.c \
    $(KERN_DIR)/interrupts.c \
    $(KERN_DIR)/string.c \
    $(KERN_DIR)/thread.c \
    $(KERN_DIR)/mutex.c  \
    $(KERN_DIR)/semaphore.c


KERN_OBJS := $(patsubst $(KERN_DIR)/%.c, $(BUILD)/kernel/%.o, $(KERN_SRCS))

KERNEL_BIN := os.bin
ISO_IMAGE  := os.iso

.PHONY: all clean run iso

all: $(KERNEL_BIN)

$(BUILD):
	mkdir -p $(BUILD)/boot $(BUILD)/kernel

$(BUILD)/boot/%.o: $(BOOT_DIR)/%.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/kernel/%.o: $(KERN_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(BOOT_OBJS) $(KERN_OBJS)
	$(LD) $(LDFLAGS) -T linker.ld $(BOOT_OBJS) $(KERN_OBJS) -o $@

iso: $(ISO_IMAGE)

$(ISO_IMAGE): $(KERNEL_BIN)
	mkdir -p $(BUILD)/isodir/boot/grub
	cp $(KERNEL_BIN) $(BUILD)/isodir/boot/$(KERNEL_BIN)
	echo 'menuentry "SENG21213 OS" {' > $(BUILD)/isodir/boot/grub/grub.cfg
	echo '  multiboot /boot/$(KERNEL_BIN)' >> $(BUILD)/isodir/boot/grub/grub.cfg
	echo '}' >> $(BUILD)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_IMAGE) $(BUILD)/isodir

run: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -display curses

clean:
	rm -rf $(BUILD) $(KERNEL_BIN) $(ISO_IMAGE)
