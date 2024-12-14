#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <list.h>

#include "vm/page.h"
#include "threads/palloc.h"
#include "threads/thread.h"

struct frame{
    struct list_elem frame_elem;

    struct thread *t;
    struct vm_entry *vme;
    void *kaddr;
    void *paddr;
};

void frame_init(void);
void *frame_alloc(enum palloc_flags, struct vm_entry *);
void frame_free(void *vaddr);

#endif