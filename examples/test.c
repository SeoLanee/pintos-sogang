#include <stdio.h>
#include <syscall.h>

int main (void)
{
    char *buf[32];
    read(0, buf, 32);
    write(1, buf, 32);

  return EXIT_SUCCESS;
}
