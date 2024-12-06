#include <list.h>

#include "threads/palloc.h"

struct frame{
    struct list_elem frame_elem;

    void *vaddr;
    void *paddr;
};

void frame_init(void);
void *frame_alloc(enum palloc_flags flags);
void frame_free(void *vaddr);

/*
    뭘 해야할까...

    일단 page fault가 나면 palloc을 한다(lazy demanding)
    
    handle_mm_fault():

    frame_get_page
        frame = get empty frame (palloc)
        if there is no remaining frame
            -> frame_page_evict (
                evict = find page to evict 
                -> swap page into disk 
                -> palloc_free () 
                )
            -> get empty frame again
        swap page into frame from disk
        modify page and swap manage table
        frame_list_insert(new frame)
        return frame;

    frame_palloc (void *upage)
    frame_evict ()
*/