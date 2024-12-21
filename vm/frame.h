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
struct frame *frame_alloc(enum palloc_flags, struct vm_entry *);
void frame_free(struct frame *);
void frame_free_with_page(struct frame *);

#endif