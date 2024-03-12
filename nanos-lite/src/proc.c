#include <proc.h>

#define MAX_NR_PROC 4

extern void naive_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(PCB *pcb, void (*entry)(void*), void *arg) {
	pcb->cp = kcontext((Area){(void*)pcb, (void*)(pcb + 1)}, entry, arg);
} 
 
void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)1L);
  context_kload(&pcb[1], hello_fun, (void*)9L);
  switch_boot_pcb();
  yield();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/hello");
  // naive_uload(NULL, "/bin/file-test");
  // naive_uload(NULL, "/bin/timer-test");
  // naive_uload(NULL, "/bin/event-test");
  // naive_uload(NULL, "/bin/bmp-test");
  // naive_uload(NULL, "/bin/nslider");
  // naive_uload(NULL, "/bin/menu");
  // naive_uload(NULL, "/bin/nterm");
  // naive_uload(NULL, "/bin/bird");
}

Context* schedule(Context *prev) {
  current->cp = prev;
	if (current == &pcb_boot) {

	} else {
		PCB* i = NULL;
		for (i = current; i < &pcb[4]; i++) {
			if (i != NULL) break;
		}
		for (i = &pcb[0]; i < current; i++) {
			if (i != NULL) break;
		}
		assert(i != NULL);
		current = i;
	}
	return current->cp;
}

