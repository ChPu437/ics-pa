#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

// #define DEBUG
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
#ifdef DEBUG
		printf("------------start context------------\n");
		// for (int i = 0; i < 32; i++) {
		//	printf("reg%d: %X\n", i, c->gpr[i]);
		// }
		printf("mcause: %d\n", c->mcause);
		printf("mstatus: %d\n", c->mstatus);
		printf("mepc: %X\n", c->mepc);
		printf("-------------end context-------------\n");
#endif
    Event ev = {0};
    switch (c->mcause) {
    	// syscall可能有很多类型，这里我们统一case到EVENT_SYSCALL，具体是哪一种syscall放到nanos_lite/do_syscall处理
			case  0: // SYS_exit
			case  1: // SYS_yield
			case  4: // SYS_write
				ev.event = EVENT_SYSCALL; break;
			case -1: ev.event = EVENT_YIELD;   break;
      default: ev.event = EVENT_ERROR;   break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
	// yield保存-1到a7, a7应该代表异常号
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
