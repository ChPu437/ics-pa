#include <common.h>

extern void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case EVENT_YIELD: /*Log("yield event detected!");*/ break;
		case EVENT_SYSCALL: Log("syscall event detected!"); do_syscall(c); break;
		case EVENT_ERROR: panic("Error event");
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
