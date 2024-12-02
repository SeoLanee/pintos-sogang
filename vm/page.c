#include <debug.h>

#include "page.h"
#include "threads/thread.h"

// #define hash_entry(HASH_ELEM, STRUCT, MEMBER)

// /* Computes and returns the hash value for hash element E, given
//    auxiliary data AUX. */
// typedef unsigned hash_hash_func (const struct hash_elem *e, void *aux);
static unsigned vm_hash_func(const struct hash_elem *e, void *aux UNUSED);

// /* Compares the value of two hash elements A and B, given
//    auxiliary data AUX.  Returns true if A is less than B, or
//    false if A is greater than or equal to B. */
// typedef bool hash_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux);
static bool vm_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED);

// /* Performs some operation on hash element E, given auxiliary
//    data AUX. */
// typedef void hash_action_func (struct hash_elem *e, void *aux);
static void vm_destroy_func (struct hash_elem *e, void *aux UNUSED);


void vm_init(struct hash *vm)
{
    return;
}


void vm_destroy(struct hash *vm)
{
    return;
}


struct vm_entry *find_vme(void *vaddr)
{
    return NULL;
}


bool insert_vme(struct hash *vm, struct vm_entry *vme)
{
    return true;
}


bool delete_vme(struct hash *vm, struct vm_entry *vme)
{
    return true;
}


/*
    //To use hash_int function on hash_hash_func format 
    unsigned hash_hash_int(const struct hash_elem *e, void *aux){
        return hash_int(e->val);
    }
*/
static unsigned vm_hash_func(const struct hash_elem *e, void *aux UNUSED)
{
    return 0;
}


/*
    //cmp function, to use hash_less_func.
    bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux){
        return a->val < b->val;
    }
 */
static bool vm_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
    return true;
}


/*
    // Remove hash element from bucket list. And free memory
    void hash_action(struct hash_elem *e, void *aux){
        struct list_elem *list_elem = &(e->list_elem);
        
        list_remove(list_elem);
        free(e);
    }
*/
static void vm_destroy_func (struct hash_elem *e, void *aux UNUSED)
{
    return;
}
