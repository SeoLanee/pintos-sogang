#include <hash.h>
#include <filesys/file.h>

enum page_status
  {
    PAGE_ELF,
    PAGE_FILE,
    PAGE_SWAP
  };

struct vm_entry
{
    struct hash_elem elem;
};

void vm_init(struct hash *);
void vm_destroy(struct hash *);

struct vm_entry *find_vme(void *);
bool insert_vme(struct hash *, struct vm_entry *);
bool delete_vme(struct hash *, struct vm_entry *);