#include <common.h>
#include "custom_syscall.h"

#define SYS_EXIT 0
#define SYS_YIELD 1
#define SYS_WRITE 4
#define SYS_BRK 9

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_EXIT:            sys_exit();  break;
		case SYS_YIELD: c->GPRx = sys_yield(); break;
		case SYS_WRITE: c->GPRx = sys_write((int)c->GPR2, (const char*)c->GPR3, (size_t)c->GPR4); break;
		case SYS_BRK:   c->GPRx = sys_brk((intptr_t)c->GPR2); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
