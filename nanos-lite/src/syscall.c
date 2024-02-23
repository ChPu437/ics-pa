#include <common.h>
#include "custom_syscall.h"
#include "syscall.h"

/* 我突然释怀的笑 
#define SYS_EXIT 0
#define SYS_YIELD 1
#define SYS_WRITE 4
#define SYS_BRK 9
*/

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_exit:            sys_exit(); break;
		case SYS_yield: c->GPRx = sys_yield(); break;
		case SYS_open:  c->GPRx = sys_open((const char*)c->GPR2, (int)c->GPR3, (int)c->GPR4); break;
		case SYS_read:  c->GPRx = sys_read((int)c->GPR2, (char*)c->GPR3, (size_t)c->GPR4); break;
		case SYS_write: c->GPRx = sys_write((int)c->GPR2, (const char*)c->GPR3, (size_t)c->GPR4); break;
		case SYS_close: c->GPRx = sys_close((int)c->GPR2); break;
		case SYS_lseek: c->GPRx = sys_lseek((int)c->GPR2, (size_t)c->GPR3, (int)c->GPR4); break;
		case SYS_brk:   c->GPRx = sys_brk((intptr_t)c->GPR2); break;
		case SYS_gettimeofday: c->GPRx = sys_gettimeofday((void*)c->GPR2, (void*)c->GPR3); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
