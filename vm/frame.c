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

struct frame *frame_alloc(enum palloc_flags flags, struct vm_entry *vme){
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
    
    return frame;
}

void frame_free(struct frame *frame){
    lock_acquire(&frame_lock);
    list_remove(&frame->frame_elem);
    lock_release(&frame_lock);

    free(frame);  
    return;
}

void frame_free_with_page(struct frame *frame){
    palloc_free_page(frame->kaddr);
    frame_free(frame);
}

static void evict_algorithm(){
        struct frame *evict = select_to_evict();
        struct thread *t = evict->t;
        struct vm_entry *vme = evict->vme;

        lock_acquire(&swap_lock);
        vme->swap_idx = swap_out(evict->kaddr);
        lock_release(&swap_lock);
        
        frame_free_with_page(evict);
        
        vme->type = PAGE_SWAP;
        vme->frame = NULL;
        pagedir_clear_page(t->pagedir, vme->uaddr);
}

struct frame *select_to_evict ()
{
    while(true){
        lock_acquire(&frame_lock);
        struct frame *victim = list_entry(list_front(&frame_list), struct frame, frame_elem);
        lock_release(&frame_lock);

        struct thread *t = victim->t;
        struct vm_entry *vme = victim->vme;
        if(pagedir_is_accessed(t->pagedir, vme->uaddr)){
            pagedir_set_accessed(t->pagedir, vme->uaddr, false);

            lock_acquire(&frame_lock);
            list_push_back(&frame_list, list_pop_front(&frame_list));
            lock_release(&frame_lock);
        }
        else return victim;
    }
}
