#include "page.h"
#include "threads/thread.h"
#include "threads/malloc.h"

#include <debug.h>

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


struct vm_entry *find_vme(void *vaddr)
{
    struct hash_elem *e;
    struct vm_entry vme;

    vme.vaddr = vaddr;

    e = hash_find(&thread_current()->vm, &vme.hash_elem);
    
    if(e)
        return hash_entry(e, struct vm_entry, hash_elem);
    else
        return NULL;
}


bool insert_vme(struct hash *vm, struct vm_entry *vme)
{
    struct hash_elem *e = hash_insert(vm, &vme->hash_elem);
    if (e)
        return false;
    else 
        return true;
}


bool delete_vme(struct hash *vm UNUSED, struct vm_entry *vme)
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
    return hash_vaddr(hash_entry(e, struct vm_entry, hash_elem)->vaddr);
}


static bool vm_less_func(const struct hash_elem *a, 
                        const struct hash_elem *b, void *aux UNUSED)
{
    return hash_entry(a, struct vm_entry, hash_elem)->vaddr 
            < hash_entry(b, struct vm_entry, hash_elem)->vaddr;
}


static void vm_destroy_func (struct hash_elem *e, void *aux UNUSED)
{
    free(hash_entry(e, struct vm_entry, hash_elem));
    return;
}