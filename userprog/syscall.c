#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

#include "devices/shutdown.h"
#include "devices/input.h"
#include "kernel/console.h"

#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

#include "filesys/filesys.h"
#include "filesys/file.h"

#include <stdio.h>
#include <syscall-nr.h>

#define FIBO_MAX 46
#define MAX(a, b) (a > b ? a : b)

static void syscall_handler (struct intr_frame *);
static void check_addr(const void *);
static int find_unusing_fd(bool using[]);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  check_addr(f->esp);
  int SYS_NUM = *(int *)f->esp;

  switch(SYS_NUM){
    case SYS_HALT: 
    halt();
    break;                   
    
    case SYS_EXIT:
    check_addr(f->esp+4);
    exit(*(int *)(f->esp+4));
    break;                   
    
    case SYS_EXEC: 
    check_addr(f->esp+4);
    f->eax=exec((char *)(f->esp+4));
    break;                   
    
    case SYS_WAIT: 
    check_addr(f->esp+4);
    f->eax=wait(*(pid_t *)(f->esp+4));
    break;                   
    
    case SYS_CREATE: 
    check_addr(f->esp+4);
    check_addr(f->esp+8);
    f->eax=create((const char *)(f->esp+4), *(unsigned *)(f->esp+8));
    break;                 
    
    case SYS_REMOVE: 
    check_addr(f->esp+4);
    f->eax=remove((const char *)(f->esp+4));
    break;                 
    
    case SYS_OPEN: 
    check_addr(f->esp+4);
    f->eax=open((const char *)(f->esp+4));
    break;                   
    
    case SYS_FILESIZE: 
    check_addr(f->esp+4);
    f->eax=filesize(*(int *)(f->esp+4));
    break;               
    
    case SYS_READ: 
    check_addr(f->esp+4);
    check_addr(f->esp+8);
    check_addr(f->esp+12);
    f->eax=read(*(int *)(f->esp+4), (void *)(f->esp+8), *(unsigned *)(f->esp+12));
    break;                   
    
    case SYS_WRITE: 
    check_addr(f->esp+4);
    check_addr(f->esp+8);
    check_addr(f->esp+12);
    f->eax=write(*(int *)(f->esp+4), (const void *)(f->esp+8), *(unsigned *)(f->esp+12));
    break;                  
    
    case SYS_SEEK: 
    check_addr(f->esp+4);
    check_addr(f->esp+8);
    seek(*(int *)(f->esp+4), *(unsigned *)(f->esp+8));
    break;                   
    
    case SYS_TELL: 
    check_addr(f->esp+4);
    f->eax=tell(*(int *)(f->esp+4));
    break;                   
    
    case SYS_CLOSE: 
    check_addr(f->esp+4);
    close(*(int *)(f->esp+4));
    break;                  

    case SYS_FIBO:
    check_addr(f->esp+4);
    f->eax = fibonacci(*(int *)(f->esp+4));
    break;

    case SYS_MAX:
    check_addr(f->esp+4);
    check_addr(f->esp+8);
    check_addr(f->esp+12);
    check_addr(f->esp+16);
    f->eax = max_of_four_int(*(int *)(f->esp+4), *(int *)(f->esp+8), *(int *)(f->esp+12), *(int *)(f->esp+16));
    break;

    default: break;
  }
}

void halt (void)
{
  shutdown_power_off();
}

void exit (int status)
{
  struct thread *t = thread_current();
  t->exit_status = status;
  printf("%s: exit(%d)\n", t->name, status);
  thread_exit();
}

pid_t exec (const char *file)
{
  check_addr(*(char **)file);

  return process_execute(*(char **)file);
}

int wait (pid_t pid)
{
  pid_t ret = process_wait(pid);
  return ret;
}

bool create (const char *file, unsigned initial_size)
{
  check_addr(*(char **)file);
  char *file_name = *(char **)file;
  return filesys_create(file_name, initial_size);
}

bool remove (const char *file)
{
  check_addr(*(char **)file);
  char *file_name = *(char **)file;
  return filesys_remove(file_name);
}

int open (const char *file)
{
  check_addr(*(char **)file);
  char *file_name = *(char **)file;
  struct file *_file = filesys_open(file_name);
  struct thread *t = thread_current();
  int idx = find_unusing_fd(t->fd_using);

  if(idx == -1 || _file == NULL) return -1;

  t->fd[idx] = _file;
  t->fd_using[idx] = true;

  return idx;
}

int filesize (int fd)
{
  struct file *file = thread_current()->fd[fd];

  return file ? file_length(file) : -1;
}

int read (int fd, void *buffer, unsigned length)
{
  check_addr(*(char **)buffer);
  char *buf = *(char **)buffer;
  int read_cnt = 0;

  if(fd == STDIN_FILENO){
    while(length--){
      *buf = (char)input_getc();
      read_cnt++;
      buf += 1;
    }
  }
  else{
    struct file *file = thread_current()->fd[fd];
    
    if(!file) return -1;
    
    read_cnt = file_read(file, buf, length);
  }

  return read_cnt;
}

int write (int fd, const void *buffer, unsigned length)
{
  check_addr(*(char **)buffer);

  char *buf = *(char **)buffer;

  if(fd == STDOUT_FILENO){
    putbuf(buf, length);
    return length;
  }
  else{
    struct file *file = thread_current()->fd[fd];
    return file ? file_write(file, buf, length) : -1;
  }
}

void seek (int fd, unsigned position)
{
  struct file *file = thread_current()->fd[fd];
  if(file) file_seek(file, position);
  return;
}

unsigned tell (int fd)
{
  struct file *file = thread_current()->fd[fd];
  return file ? file_tell(file) : -1;
}

void close (int fd)
{
  struct thread *t = thread_current();
  struct file *file = t->fd[fd];

  if(file) file_close(t->fd[fd]);
  t->fd[fd] = NULL;
  t->fd_using[fd] = false;

  return;
}

int fibonacci (int n)
{
  int i, fibo[FIBO_MAX+1] = {0, 1, };

  if (n > FIBO_MAX) return -1;
  
  for(i = 2; i <= n; i++){
    fibo[i] = fibo[i-1] + fibo[i-2];
  }

  return fibo[n];
}

int max_of_four_int(int a, int b, int c, int d)
{
  return MAX(MAX(a, b), MAX(c, d));
}

static void 
check_addr(const void *vaddr){
  if(vaddr == NULL || is_kernel_vaddr(vaddr) || 
    pagedir_get_page(thread_current()->pagedir, vaddr) == NULL) 
  {
    exit(-1);
  }
}

static int
find_unusing_fd(bool using[]){
  for(int i = 0; i < FD_MAX; i++){
    if(!using[i]) return i;
  }
  
  return -1;
}