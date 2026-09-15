#ifndef FS_H
#define FS_H

#include "types.h"

/*
 * SENG21213-OS - Stage 4
 * RAM Disk File System
 *
 * Lecture L12:
 * Simple flat filesystem stored on a RAM disk.
 */

#define FS_MAGIC 0x53454E47
#define FS_BLOCK_SIZE 4096

#define FS_MAX_INODES 64
#define FS_MAX_FILENAME 28
#define FS_DIRECT_BLOCKS 8
#define FS_MAX_FD 16

#define FS_DIRECTORY_BLOCK 1
#define FS_BLOCK_BITMAP_BLOCK 2
#define FS_INODE_BITMAP_BLOCK 3
#define FS_INODE_TABLE_BLOCK 4
#define FS_DATA_START_BLOCK 5

typedef struct {
    uint32_t magic;
    uint32_t block_count;
    uint32_t inode_count;
    uint32_t block_size;
    uint32_t data_start;
} superblock_t;

typedef struct {
    uint32_t size;
    uint32_t direct_blocks[FS_DIRECT_BLOCKS];
} inode_t;

typedef struct {
    char name[FS_MAX_FILENAME];
    uint32_t inode;
} directory_entry_t;

/* Initialize the filesystem. */
void fs_init(void);

/* Open a file. create = 1 creates it if it does not exist. */
int fs_open(const char *name, int create);

/* Read data from an opened file. */
int fs_read(int fd, void *buffer, uint32_t size);

/* Write data to an opened file. */
int fs_write(int fd, const void *buffer, uint32_t size);

/* Close an opened file. */
int fs_close(int fd);

/* Delete a file. */
int fs_unlink(const char *name);

/* List files in the filesystem. */
void fs_list(void);

#endif
