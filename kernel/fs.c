#include "fs.h"
#include "ramdisk.h"
#include "vga.h"

/*
 * SENG21213-OS - Stage 4
 * RAM Disk File System
 *
 * Lecture L12:
 * A simple flat filesystem using a superblock,
 * block bitmap, inode bitmap, inode table,
 * and directory entries.
 */

#define FS_DIR_ENTRIES 64
#define FS_FD_UNUSED 0

typedef struct {
    uint32_t inode;
    uint32_t position;
    uint32_t used;
} file_descriptor_t;

static superblock_t superblock;
static uint8_t block_bitmap[RAMDISK_BLOCK_COUNT / 8];
static uint8_t inode_bitmap[FS_MAX_INODES / 8];

static directory_entry_t directory[FS_DIR_ENTRIES];
static inode_t inode_table[FS_MAX_INODES];

static file_descriptor_t fd_table[FS_MAX_FD];

/* ---------- Basic memory helpers ---------- */

static void fs_memset(void *dest, uint8_t value, uint32_t size)
{
    uint8_t *p = (uint8_t *)dest;

    for (uint32_t i = 0; i < size; i++) {
        p[i] = value;
    }
}

static void fs_memcpy(void *dest, const void *src, uint32_t size)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    for (uint32_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

static int fs_strcmp(const char *a, const char *b)
{
    uint32_t i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 1;
        }

        i++;
    }

    return a[i] == b[i] ? 0 : 1;
}

static void fs_strcpy(char *dest, const char *src)
{
    uint32_t i = 0;

    while (src[i] != '\0' && i < FS_MAX_FILENAME - 1) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    while (i < FS_MAX_FILENAME) {
        dest[i] = '\0';
        i++;
    }
}

/* ---------- Bitmap helpers ---------- */

static void bitmap_set(uint8_t *bitmap, uint32_t index)
{
    bitmap[index / 8] |= (uint8_t)(1U << (index % 8));
}

static void bitmap_clear(uint8_t *bitmap, uint32_t index)
{
    bitmap[index / 8] &= (uint8_t)~(1U << (index % 8));
}

static int bitmap_test(const uint8_t *bitmap, uint32_t index)
{
    return (bitmap[index / 8] & (uint8_t)(1U << (index % 8))) != 0;
}

/* ---------- Disk metadata ---------- */

static void save_metadata(void)
{
    uint8_t buffer[FS_BLOCK_SIZE];

    fs_memset(buffer, 0, FS_BLOCK_SIZE);
    fs_memcpy(buffer, &superblock, sizeof(superblock));
    ramdisk_write(0, buffer);

    fs_memset(buffer, 0, FS_BLOCK_SIZE);
    fs_memcpy(buffer, directory, sizeof(directory));
    ramdisk_write(FS_DIRECTORY_BLOCK, buffer);

    fs_memset(buffer, 0, FS_BLOCK_SIZE);
    fs_memcpy(buffer, block_bitmap, sizeof(block_bitmap));
    ramdisk_write(FS_BLOCK_BITMAP_BLOCK, buffer);

    fs_memset(buffer, 0, FS_BLOCK_SIZE);
    fs_memcpy(buffer, inode_bitmap, sizeof(inode_bitmap));
    ramdisk_write(FS_INODE_BITMAP_BLOCK, buffer);

    fs_memset(buffer, 0, FS_BLOCK_SIZE);
    fs_memcpy(buffer, inode_table, sizeof(inode_table));
    ramdisk_write(FS_INODE_TABLE_BLOCK, buffer);
}


/* ---------- Allocation ---------- */

static int allocate_inode(void)
{
    for (uint32_t i = 0; i < FS_MAX_INODES; i++) {
        if (!bitmap_test(inode_bitmap, i)) {
            bitmap_set(inode_bitmap, i);
            return (int)i;
        }
    }

    return -1;
}

static void free_inode(uint32_t inode)
{
    if (inode < FS_MAX_INODES) {
        bitmap_clear(inode_bitmap, inode);
    }
}

static int allocate_block(void)
{
    for (uint32_t i = FS_DATA_START_BLOCK;
         i < RAMDISK_BLOCK_COUNT;
         i++) {

        if (!bitmap_test(block_bitmap, i)) {
            bitmap_set(block_bitmap, i);
            return (int)i;
        }
    }

    return -1;
}

static void free_block(uint32_t block)
{
    if (block >= FS_DATA_START_BLOCK &&
        block < RAMDISK_BLOCK_COUNT) {

        bitmap_clear(block_bitmap, block);
    }
}

/* ---------- Directory helpers ---------- */

static int find_directory_entry(const char *name)
{
    for (uint32_t i = 0; i < FS_DIR_ENTRIES; i++) {
        if (directory[i].name[0] != '\0' &&
            fs_strcmp(directory[i].name, name) == 0) {

            return (int)i;
        }
    }

    return -1;
}

static int find_free_directory_entry(void)
{
    for (uint32_t i = 0; i < FS_DIR_ENTRIES; i++) {
        if (directory[i].name[0] == '\0') {
            return (int)i;
        }
    }

    return -1;
}

/* ---------- Filesystem initialization ---------- */

void fs_init(void)
{
    ramdisk_init();

    fs_memset(&superblock, 0, sizeof(superblock));
    fs_memset(block_bitmap, 0, sizeof(block_bitmap));
    fs_memset(inode_bitmap, 0, sizeof(inode_bitmap));
    fs_memset(directory, 0, sizeof(directory));
    fs_memset(inode_table, 0, sizeof(inode_table));
    fs_memset(fd_table, 0, sizeof(fd_table));

    superblock.magic = FS_MAGIC;
    superblock.block_count = RAMDISK_BLOCK_COUNT;
    superblock.inode_count = FS_MAX_INODES;
    superblock.block_size = FS_BLOCK_SIZE;
    superblock.data_start = FS_DATA_START_BLOCK;

    /*
     * Reserve filesystem metadata blocks.
     * Blocks 0-4 are not available for file data.
     */
    for (uint32_t i = 0; i < FS_DATA_START_BLOCK; i++) {
        bitmap_set(block_bitmap, i);
    }

    save_metadata();
}

/* ---------- Open ---------- */

int fs_open(const char *name, int create)
{
    if (name == 0 || name[0] == '\0') {
        return -1;
    }

    int dir_index = find_directory_entry(name);

    if (dir_index < 0 && create) {
        int inode_index = allocate_inode();

        if (inode_index < 0) {
            return -1;
        }

        int new_dir = find_free_directory_entry();

        if (new_dir < 0) {
            free_inode((uint32_t)inode_index);
            return -1;
        }

        fs_memset(&inode_table[inode_index], 0, sizeof(inode_t));

        fs_strcpy(directory[new_dir].name, name);
        directory[new_dir].inode = (uint32_t)inode_index;

        dir_index = new_dir;

        save_metadata();
    }

    if (dir_index < 0) {
        return -1;
    }

    uint32_t inode = directory[dir_index].inode;

    for (uint32_t fd = 0; fd < FS_MAX_FD; fd++) {
        if (!fd_table[fd].used) {
            fd_table[fd].used = 1;
            fd_table[fd].inode = inode;
            fd_table[fd].position = 0;

            return (int)fd;
        }
    }

    return -1;
}

/* ---------- Read ---------- */

int fs_read(int fd, void *buffer, uint32_t size)
{
    if (fd < 0 || fd >= FS_MAX_FD ||
        buffer == 0 || !fd_table[fd].used) {
        return -1;
    }

    uint32_t inode_index = fd_table[fd].inode;
    inode_t *inode = &inode_table[inode_index];

    uint32_t position = fd_table[fd].position;

    if (position >= inode->size) {
        return 0;
    }

    uint32_t remaining = inode->size - position;

    if (size > remaining) {
        size = remaining;
    }

    uint8_t block_buffer[FS_BLOCK_SIZE];
    uint8_t *output = (uint8_t *)buffer;
    uint32_t total = 0;

    while (total < size) {
        uint32_t current_position = position + total;
        uint32_t block_index = current_position / FS_BLOCK_SIZE;
        uint32_t offset = current_position % FS_BLOCK_SIZE;

        if (block_index >= FS_DIRECT_BLOCKS) {
            break;
        }

        uint32_t block = inode->direct_blocks[block_index];

        if (block == 0) {
            break;
        }

        ramdisk_read(block, block_buffer);

        uint32_t chunk = FS_BLOCK_SIZE - offset;

        if (chunk > size - total) {
            chunk = size - total;
        }

        fs_memcpy(output + total, block_buffer + offset, chunk);

        total += chunk;
    }

    fd_table[fd].position += total;

    return (int)total;
}

/* ---------- Write ---------- */

int fs_write(int fd, const void *buffer, uint32_t size)
{
    if (fd < 0 || fd >= FS_MAX_FD ||
        buffer == 0 || !fd_table[fd].used) {
        return -1;
    }

    uint32_t inode_index = fd_table[fd].inode;
    inode_t *inode = &inode_table[inode_index];

    uint32_t position = fd_table[fd].position;
    uint32_t written = 0;

    if (position >= FS_DIRECT_BLOCKS * FS_BLOCK_SIZE) {
        return -1;
    }

    if (size > (FS_DIRECT_BLOCKS * FS_BLOCK_SIZE) - position) {
        size = (FS_DIRECT_BLOCKS * FS_BLOCK_SIZE) - position;
    }

    uint8_t block_buffer[FS_BLOCK_SIZE];
    const uint8_t *input = (const uint8_t *)buffer;

    while (written < size) {
        uint32_t current_position = position + written;
        uint32_t block_index = current_position / FS_BLOCK_SIZE;
        uint32_t offset = current_position % FS_BLOCK_SIZE;

        if (block_index >= FS_DIRECT_BLOCKS) {
            break;
        }

        if (inode->direct_blocks[block_index] == 0) {
            int new_block = allocate_block();

            if (new_block < 0) {
                break;
            }

            inode->direct_blocks[block_index] = (uint32_t)new_block;

            fs_memset(block_buffer, 0, FS_BLOCK_SIZE);
        } else {
            ramdisk_read(inode->direct_blocks[block_index], block_buffer);
        }

        uint32_t chunk = FS_BLOCK_SIZE - offset;

        if (chunk > size - written) {
            chunk = size - written;
        }

        fs_memcpy(block_buffer + offset, input + written, chunk);

        ramdisk_write(inode->direct_blocks[block_index], block_buffer);

        written += chunk;
    }

    position += written;
    fd_table[fd].position = position;

    if (position > inode->size) {
        inode->size = position;
    }

    save_metadata();

    return (int)written;
}

/* ---------- Close ---------- */

int fs_close(int fd)
{
    if (fd < 0 || fd >= FS_MAX_FD ||
        !fd_table[fd].used) {
        return -1;
    }

    fd_table[fd].used = FS_FD_UNUSED;
    fd_table[fd].inode = 0;
    fd_table[fd].position = 0;

    return 0;
}

/* ---------- Delete ---------- */

int fs_unlink(const char *name)
{
    if (name == 0) {
        return -1;
    }

    int dir_index = find_directory_entry(name);

    if (dir_index < 0) {
        return -1;
    }

    uint32_t inode_index = directory[dir_index].inode;
    inode_t *inode = &inode_table[inode_index];

    for (uint32_t i = 0; i < FS_DIRECT_BLOCKS; i++) {
        if (inode->direct_blocks[i] != 0) {
            free_block(inode->direct_blocks[i]);
            inode->direct_blocks[i] = 0;
        }
    }

    inode->size = 0;

    free_inode(inode_index);

    fs_memset(&directory[dir_index], 0, sizeof(directory_entry_t));

    save_metadata();

    return 0;
}

/* ---------- Directory listing ---------- */

void fs_list(void)
{
    int found = 0;

    vga_puts("NAME                         SIZE\n");
    vga_puts("---------------------------- -----\n");

    for (uint32_t i = 0; i < FS_DIR_ENTRIES; i++) {
        if (directory[i].name[0] != '\0') {
            uint32_t inode_index = directory[i].inode;

            vga_puts(directory[i].name);
            vga_puts("                         ");
            vga_put_uint(inode_table[inode_index].size);
            vga_puts("\n");

            found = 1;
        }
    }

    if (!found) {
        vga_puts("(empty)\n");
    }
}
