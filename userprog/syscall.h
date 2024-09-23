#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <debug.h>

typedef int pid_t;
#define PID_ERROR ((pid_t) -1)

#define EXIT_SUCCESS 0          
#define EXIT_FAILURE 1          


void syscall_init (void);

void halt (void) NO_RETURN;
void exit (int status) NO_RETURN;
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size UNUSED);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd UNUSED);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd UNUSED, unsigned position UNUSED);
unsigned tell (int fd UNUSED);
void close (int fd UNUSED);

int fibonacci (int n);
int max_of_four_int(int a, int b, int c, int d);

#endif /* userprog/syscall.h */
