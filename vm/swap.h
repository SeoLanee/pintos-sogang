#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "threads/synch.h"
#include "devices/block.h"

void swap_init(void);
block_sector_t swap_out(void *);
void swap_in(void *, block_sector_t);

struct lock swap_lock;

#endif