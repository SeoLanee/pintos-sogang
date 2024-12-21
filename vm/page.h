#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>

#include "vm/frame.h"
#include "filesys/file.h"
#include "devices/block.h"

enum page_type
  {
    PAGE_MEM,
    PAGE_SWAP,
    PAGE_BIN,
    PAGE_FILE
  };



struct vm_entry
{
    struct hash_elem hash_elem;
    enum page_type type;
    void *uaddr;
    bool writable;

    // MEM
    struct frame *frame;

    // BIN
    struct file *file;
    off_t ofs;
    size_t read_bytes;
    size_t zero_bytes;

    // SWAP
    block_sector_t swap_idx;
};

void vm_init(struct hash *);
void vm_destroy(struct hash *);

struct vm_entry *vm_create_vme(void);
void vm_free_vme(struct vm_entry *);

struct vm_entry *vm_find_vme(void *);
bool vm_insert_vme(struct hash *, struct vm_entry *);
bool vm_delete_vme(struct hash *, struct vm_entry *);

bool load_file(void *, struct vm_entry *);

#endif