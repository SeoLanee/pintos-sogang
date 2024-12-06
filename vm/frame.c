#include <list.h>
#include "vm/frame.h"

#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"

static struct list frame_list;
static struct lock frame_lock;

void frame_init(){
    list_init(&frame_list);
    lock_init(&frame_lock);
}

void *frame_alloc(enum palloc_flags flags){
    void *kpage;
    
    kpage = palloc_get_page(PAL_USER | flags);
    if(kpage ==NULL)
        return NULL;

    struct frame *frame = (struct frame *)malloc(sizeof (struct frame));
    frame->vaddr = kpage; 
    frame->paddr = (void *)vtop(kpage);

    lock_acquire(&frame_lock);
    list_push_back(&frame_list, &frame->frame_elem);
    lock_release(&frame_lock);

    return kpage;
}

void frame_free(void *vaddr){
    struct list_elem *e;

    for(e = list_begin(&frame_list); e != list_end(&frame_list); e = list_next(e)){
        if(list_entry(e, struct frame, frame_elem)->vaddr == vaddr)
            break;
    }

    if(e == list_end(&frame_list))
        PANIC("there is no frame for virtual address");
    else{
        lock_acquire(&frame_lock);
        list_remove(e);
        lock_release(&frame_lock);
    }
    
    palloc_free_page(vaddr);
    return;
}