#include "pmm.h"

/*
 * SENG21213-OS - Stage 3
 * Physical Memory Manager
 *
 * Lecture L11:
 * - Uses the Multiboot memory map supplied by the bootloader.
 * - One bitmap bit represents one 4 KB physical page frame.
 * - 1 = used, 0 = free.
 */

#define PMM_MAX_FRAMES 1048576
#define PMM_BITMAP_SIZE (PMM_MAX_FRAMES / 8)

#define MULTIBOOT_MEMORY_AVAILABLE 1

typedef struct {
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} multiboot_mmap_entry_t;

/* Bitmap is stored inside the kernel BSS area. */
static uint8_t frame_bitmap[PMM_BITMAP_SIZE];

/* Number of usable physical page frames. */
static uint32_t total_frames = 0;

/* Number of currently free usable frames. */
static uint32_t free_frames_count = 0;

/* Highest physical frame index that the bitmap may contain. */
static uint32_t max_frame = 0;

extern uint32_t _kernel_end;

/* Mark a physical frame as used. */
static void frame_set(uint32_t frame)
{
    frame_bitmap[frame / 8] |= (uint8_t)(1U << (frame % 8));
}

/* Mark a physical frame as free. */
static void frame_clear(uint32_t frame)
{
    frame_bitmap[frame / 8] &=
        (uint8_t)~(1U << (frame % 8));
}

/* Check whether a physical frame is used. */
static int frame_is_used(uint32_t frame)
{
    return (frame_bitmap[frame / 8] &
            (uint8_t)(1U << (frame % 8))) != 0;
}

void pmm_init(uint32_t multiboot_info)
{
    uint32_t i;

    /*
     * Initially mark every physical frame as USED.
     * Only frames inside Multiboot "available" regions
     * will be released.
     */
    for (i = 0; i < PMM_BITMAP_SIZE; i++) {
        frame_bitmap[i] = 0xFF;
    }

    total_frames = 0;
    free_frames_count = 0;
    max_frame = 0;

    /*
     * Multiboot information structure:
     * offset 0  = flags
     * offset 44 = mmap_length
     * offset 48 = mmap_addr
     */
    uint32_t *mb = (uint32_t *)multiboot_info;
    uint32_t flags = mb[0];

    /*
     * Multiboot flag bit 6 indicates that the memory map exists.
     */
    if ((flags & (1U << 6)) == 0) {
        return;
    }

    uint32_t mmap_length = mb[11];
    uint32_t mmap_addr = mb[12];

    uint32_t offset = 0;

    while (offset < mmap_length) {

        multiboot_mmap_entry_t *entry =
            (multiboot_mmap_entry_t *)(mmap_addr + offset);

        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {

            uint64_t start =
                ((uint64_t)entry->addr_high << 32) |
                entry->addr_low;

            uint64_t length =
                ((uint64_t)entry->len_high << 32) |
                entry->len_low;

            uint64_t end = start + length;

            /*
             * Convert the available byte range into
             * 4 KB physical page frame numbers.
             */
            uint64_t first_frame =
                (start + PMM_PAGE_SIZE - 1) /
                PMM_PAGE_SIZE;

            uint64_t last_frame =
                end / PMM_PAGE_SIZE;

            if (first_frame >= PMM_MAX_FRAMES) {
                offset += entry->size + sizeof(uint32_t);
                continue;
            }

            if (last_frame > PMM_MAX_FRAMES) {
                last_frame = PMM_MAX_FRAMES;
            }

            /*
             * Release every frame belonging to this
             * available memory region.
             */
            for (uint64_t frame = first_frame;
                 frame < last_frame;
                 frame++) {

                uint32_t f = (uint32_t)frame;

                if (frame_is_used(f)) {
                    frame_clear(f);
                    total_frames++;
                    free_frames_count++;
                }

                if (f + 1 > max_frame) {
                    max_frame = f + 1;
                }
            }
        }

        /*
         * Multiboot memory-map entries contain a size field
         * followed by the actual entry data.
         */
        offset += entry->size + sizeof(uint32_t);
    }

    /*
     * Reserve the first 1 MB.
     * This contains BIOS/reserved memory and VGA memory.
     */
    uint32_t reserved_frames =
        (1024 * 1024) / PMM_PAGE_SIZE;

    for (i = 0; i < reserved_frames && i < PMM_MAX_FRAMES; i++) {

        if (!frame_is_used(i)) {
            frame_set(i);

            if (free_frames_count > 0) {
                free_frames_count--;
            }
        }
    }

    /*
     * Reserve all physical frames occupied by the kernel.
     */
    uint32_t kernel_end = (uint32_t)&_kernel_end;

    uint32_t kernel_last_frame =
        (kernel_end + PMM_PAGE_SIZE - 1) /
        PMM_PAGE_SIZE;

    for (i = 0;
         i < kernel_last_frame && i < PMM_MAX_FRAMES;
         i++) {

        if (!frame_is_used(i)) {
            frame_set(i);

            if (free_frames_count > 0) {
                free_frames_count--;
            }
        }
    }
}

uint32_t pmm_alloc_frame(void)
{
    /*
     * First-fit scan through the complete physical
     * frame range represented by the memory map.
     */
    for (uint32_t frame = 0;
         frame < max_frame;
         frame++) {

        if (!frame_is_used(frame)) {

            frame_set(frame);

            if (free_frames_count > 0) {
                free_frames_count--;
            }

            return frame * PMM_PAGE_SIZE;
        }
    }

    /* No free physical frame available. */
    return 0;
}

void pmm_free_frame(uint32_t paddr)
{
    if (paddr == 0 ||
        (paddr % PMM_PAGE_SIZE) != 0) {
        return;
    }

    uint32_t frame = paddr / PMM_PAGE_SIZE;

    if (frame >= max_frame ||
        frame >= PMM_MAX_FRAMES) {
        return;
    }

    /*
     * Only free a frame that is currently marked used.
     */
    if (frame_is_used(frame)) {
        frame_clear(frame);
        free_frames_count++;
    }
}

uint32_t pmm_total_memory(void)
{
    return total_frames * PMM_PAGE_SIZE;
}

uint32_t pmm_free_frames(void)
{
    return free_frames_count;
}
