#include "ramdisk.h"

/*
 * SENG21213-OS - Stage 4
 * RAM Disk
 *
 * Lecture L12:
 * The RAM disk is a fixed 1 MB byte array.
 *
 * It is declared static so the linker places it in the
 * kernel's BSS/data area rather than requiring disk storage.
 */

static uint8_t ramdisk[RAMDISK_SIZE];

void ramdisk_init(void)
{
    /*
     * Clear the complete RAM disk.
     * A zero-filled disk represents an empty filesystem.
     */
    for (uint32_t i = 0; i < RAMDISK_SIZE; i++) {
        ramdisk[i] = 0;
    }
}

int ramdisk_read(uint32_t block, void *buffer)
{
    if (buffer == 0 || block >= RAMDISK_BLOCK_COUNT) {
        return -1;
    }

    uint32_t offset = block * RAMDISK_BLOCK_SIZE;

    for (uint32_t i = 0; i < RAMDISK_BLOCK_SIZE; i++) {
        ((uint8_t *)buffer)[i] = ramdisk[offset + i];
    }

    return 0;
}

int ramdisk_write(uint32_t block, const void *buffer)
{
    if (buffer == 0 || block >= RAMDISK_BLOCK_COUNT) {
        return -1;
    }

    uint32_t offset = block * RAMDISK_BLOCK_SIZE;

    for (uint32_t i = 0; i < RAMDISK_BLOCK_SIZE; i++) {
        ramdisk[offset + i] = ((const uint8_t *)buffer)[i];
    }

    return 0;
}

uint32_t ramdisk_block_count(void)
{
    return RAMDISK_BLOCK_COUNT;
}
