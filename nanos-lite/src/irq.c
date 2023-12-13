#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case EVENT_YIELD: printf("yield event detected!\n"); break;
		case EVENT_SYSCALL: printf("syscall event detected!\n"); assert(0); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
