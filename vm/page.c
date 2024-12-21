#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"

#include <debug.h>
#include <stdio.h>
#include <string.h>



static unsigned vm_hash_func (const struct hash_elem *e, void *aux UNUSED);
static bool vm_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED);
static void vm_destroy_func (struct hash_elem *e, void *aux UNUSED);
static unsigned hash_vaddr (void *vaddr); 


void vm_init(struct hash *vm)
{
    hash_init(vm, vm_hash_func, vm_less_func, NULL);
    return;
}

void vm_destroy(struct hash *vm)
{
    hash_destroy(vm, vm_destroy_func);
    return;
}

struct vm_entry *vm_create_vme()
{
    struct vm_entry *vme = (struct vm_entry *) malloc(sizeof (struct vm_entry));
    
    if(vme == NULL)
        PANIC("Failed to get vm entry");

    return vme;
}

void vm_free_vme(struct vm_entry *vme)
{
    switch(vme->type){
        case PAGE_MEM: 
            frame_free(vme->frame);
            break;
        case PAGE_SWAP:
            swap_free(vme->swap_idx);
            break;
        default: break;
    }
    free(vme);
    return;
}

struct vm_entry *vm_find_vme(void *uaddr)
{
    struct hash_elem *e;
    struct vm_entry vme;
    
    vme.uaddr = pg_round_down(uaddr);

    e = hash_find(&thread_current()->vm, &vme.hash_elem);
    
    if(e)
        return hash_entry(e, struct vm_entry, hash_elem);
    else
        return NULL;
}

bool vm_insert_vme(struct hash *vm, struct vm_entry *vme)
{
    struct hash_elem *e = hash_insert(vm, &vme->hash_elem);
    if (!e)
        return true;
    else 
        return false;
}

bool vm_delete_vme(struct hash *vm UNUSED, struct vm_entry *vme)
{
    struct hash_elem *e = hash_delete(vm, &vme->hash_elem);
    if (e) {
        free(e);
        return true;
    }
    else
        return false;
}

static unsigned
hash_vaddr (void *vaddr) 
{
  return hash_bytes (&vaddr, sizeof vaddr);
}

static unsigned vm_hash_func(const struct hash_elem *e, void *aux UNUSED)
{
    return hash_vaddr(hash_entry(e, struct vm_entry, hash_elem)->uaddr);
}

static bool vm_less_func(const struct hash_elem *a, 
                        const struct hash_elem *b, void *aux UNUSED)
{
    return hash_entry(a, struct vm_entry, hash_elem)->uaddr 
            < hash_entry(b, struct vm_entry, hash_elem)->uaddr;
}

static void vm_destroy_func (struct hash_elem *e, void *aux UNUSED)
{
    struct vm_entry *vme = hash_entry(e, struct vm_entry, hash_elem);
    switch(vme->type){
        case PAGE_MEM: 
            frame_free(vme->frame);
            break;
        case PAGE_SWAP:
            swap_free(vme->swap_idx);
            break;
        default: break;
    }

    free(vme);
    return;
}


bool load_file(void *kaddr, struct vm_entry *vme)
{
    struct file *file = vme->file;
    off_t ofs = vme->ofs;
    size_t page_read_bytes = vme->read_bytes;
    size_t page_zero_bytes = vme->zero_bytes;

    if(!lock_held_by_current_thread(&filesys_lock))
        lock_acquire(&filesys_lock);
    
    off_t read_bytes = file_read_at (file, kaddr, page_read_bytes, ofs);

    if(!lock_keep_hold) 
        lock_release(&filesys_lock);

    if (read_bytes != (int) page_read_bytes)
        return false;
    
    memset (kaddr + page_read_bytes, 0, page_zero_bytes);
    return true;
}
