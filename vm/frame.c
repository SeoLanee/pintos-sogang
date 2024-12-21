#include <list.h>

#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"
#include "vm/swap.h"

#include <stdio.h>

static struct list frame_list;
static struct lock frame_lock;

static void evict_algorithm(void);
static struct frame *select_to_evict (void);

void frame_init(){
    list_init(&frame_list);
    lock_init(&frame_lock);
}

void *frame_alloc(enum palloc_flags flags, struct vm_entry *vme){
    void *kpage;
    
    kpage = palloc_get_page(PAL_USER | flags);
    if(kpage == NULL){
        evict_algorithm();
        return frame_alloc(flags, vme);
    }

    struct frame *frame = (struct frame *)malloc(sizeof (struct frame));
    frame->t = thread_current();
    frame->vme = vme;
    frame->kaddr = kpage; 
    frame->paddr = (void *)vtop(kpage);

    lock_acquire(&frame_lock);
    list_push_back(&frame_list, &frame->frame_elem);
    lock_release(&frame_lock);
    return kpage;
}

void frame_free(void *kaddr){
    struct list_elem *e;

    lock_acquire(&frame_lock);
    for(e = list_begin(&frame_list); e != list_end(&frame_list); e = list_next(e)){
        if(list_entry(e, struct frame, frame_elem)->kaddr == kaddr)
            break;
    }
    lock_release(&frame_lock);

    if (e != list_end(&frame_list)) {
        lock_acquire(&frame_lock);
        list_remove(e);
        lock_release(&frame_lock);
        palloc_free_page(kaddr);
    }
    
    return;
}


static void evict_algorithm(){
        struct frame *evict = select_to_evict();
        evict->vme->type = PAGE_SWAP;
        pagedir_clear_page(evict->t->pagedir, evict->vme->uaddr);
        
        lock_acquire(&swap_lock);
        evict->vme->swap_idx = swap_out(evict->kaddr);
        lock_release(&swap_lock);
        
        palloc_free_page(evict->kaddr);
}

struct frame *select_to_evict ()
{
    while(true){
        lock_acquire(&frame_lock);
        struct frame *victim = list_entry(list_pop_front(&frame_list), struct frame, frame_elem);
        lock_release(&frame_lock);

        struct thread *t = victim->t;
        struct vm_entry *vme = victim->vme;
        if(pagedir_is_accessed(t->pagedir, vme->uaddr)){
            pagedir_set_accessed(t->pagedir, vme->uaddr, false);
            
            lock_acquire(&frame_lock);
            list_push_back(&frame_list, &victim->frame_elem);
            lock_release(&frame_lock);
        }
        else return victim;
    }
}
