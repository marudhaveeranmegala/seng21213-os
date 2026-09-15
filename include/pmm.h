#ifndef PMM_H
#define PMM_H

#include "types.h"

/* Physical page size: 4 KB */
#define PMM_PAGE_SIZE 4096

/* Initialize the Physical Memory Manager */
void pmm_init(uint32_t multiboot_info);

/* Allocate one physical page frame */
uint32_t pmm_alloc_frame(void);

/* Free a previously allocated physical page frame */
void pmm_free_frame(uint32_t paddr);

/* Return total managed memory in bytes */
uint32_t pmm_total_memory(void);

/* Return number of free page frames */
uint32_t pmm_free_frames(void);

#endif
