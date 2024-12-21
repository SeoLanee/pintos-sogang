#include "swap.h"

#include "threads/vaddr.h"
#include <bitmap.h>

#define SECTORS_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

static struct block *swap_block;
static struct bitmap *swap_pool;
static struct lock bitmap_lock;

static size_t swap_available;

void swap_init(void)
{
    swap_block = block_get_role(BLOCK_SWAP);
    if (!swap_block) {
        PANIC ("Failed to get swap block\n");
        return;
    }
    swap_available = block_size(swap_block) / SECTORS_PER_PAGE;
    swap_pool = bitmap_create(swap_available);
    bitmap_set_all(swap_pool, false);
    
    lock_init(&bitmap_lock);
    lock_init(&swap_lock);

    return;
}


block_sector_t swap_out(void *page)
{
    ASSERT (page != NULL)

    lock_acquire(&bitmap_lock);
    size_t swap_idx = bitmap_scan_and_flip(swap_pool, 0, 1, false);
    lock_release(&bitmap_lock);

    if(swap_idx == BITMAP_ERROR){
        PANIC ("failed to get free slot");
    }

    for(int i = 0; i < SECTORS_PER_PAGE; i++){
        block_sector_t sector = swap_idx * SECTORS_PER_PAGE + i;
        size_t offset = BLOCK_SECTOR_SIZE * i;
        block_write (swap_block, sector, page + offset);
    }

    return swap_idx;
}


void swap_in(void *page, block_sector_t swap_idx)
{
    ASSERT(bitmap_test(swap_pool, swap_idx) == true)

    for(int i = 0; i < SECTORS_PER_PAGE; i++){
        block_sector_t sector = swap_idx * SECTORS_PER_PAGE + i;
        size_t offset = BLOCK_SECTOR_SIZE * i;
        block_read (swap_block, sector, page + offset);
    }

    bitmap_set(swap_pool, swap_idx, false);
    return;
}

void swap_free(block_sector_t swap_idx)
{
    ASSERT(bitmap_test(swap_pool, swap_idx) == true)

    bitmap_set(swap_pool, swap_idx, false);
    return;
}
