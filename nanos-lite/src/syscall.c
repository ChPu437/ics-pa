#include <common.h>
#include "syscall.h"

#define SYS_EXIT 0
#define SYS_YIELD 1
#define SYS_WRITE 4
#define SYS_BRK 11

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_EXIT:  sys_exit(c);  break;
		case SYS_YIELD: sys_yield(c); break;
		case SYS_WRITE: sys_write(c); break;
		case SYS_BRK:   sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
