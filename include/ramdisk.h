#ifndef RAMDISK_H
#define RAMDISK_H

#include "types.h"

/*
 * SENG21213-OS - Stage 4
 * RAM Disk
 *
 * Lecture L12:
 * A 1 MB RAM disk stored as a fixed-size byte array.
 */

#define RAMDISK_SIZE (1024 * 1024)
#define RAMDISK_BLOCK_SIZE 4096
#define RAMDISK_BLOCK_COUNT (RAMDISK_SIZE / RAMDISK_BLOCK_SIZE)

/* Initialize the RAM disk. */
void ramdisk_init(void);

/* Read one 4 KB block from the RAM disk. */
int ramdisk_read(uint32_t block, void *buffer);

/* Write one 4 KB block to the RAM disk. */
int ramdisk_write(uint32_t block, const void *buffer);

/* Return the total number of blocks. */
uint32_t ramdisk_block_count(void);

#endif
