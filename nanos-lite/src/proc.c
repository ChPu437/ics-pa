#include <proc.h>

#define MAX_NR_PROC 4

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_uload(PCB*, const char*);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
static bool pcb_used[MAX_NR_PROC] = {};

void context_kload(PCB *_pcb, void (*entry)(void*), void *arg) {
	assert(entry);
	for (int i = 0; i < MAX_NR_PROC; i++) {
		if (_pcb == &pcb[i]) {
			pcb_used[i] = 1;
			break;
		}
	}
	_pcb->cp = kcontext((Area){(void*)_pcb, (void*)((uintptr_t)_pcb + sizeof(PCB))}, entry, arg);
	// 																												to avoid out-of-bound
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
  Log("Initializing processes...");
  // context_kload(&pcb[0], hello_fun, (void*)1L);
  // context_kload(&pcb[1], hello_fun, (void*)9L);
  context_uload(&pcb[0], "/bin/nslider");
  switch_boot_pcb();


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
	/* if (current == &pcb[MAX_NR_PROC - 1]) {
		printf("1\n");
		current = &pcb[0];
	} else if (current == &pcb_boot) {
		printf("2\n");
		current = &pcb[0];
	} else if () {
		printf("3\n");
		current = &pcb[0];
	} else {
		printf("4\n");
		current = current + 1;
	} */
	if (current == &pcb_boot) {
		for (int i = 0; i < MAX_NR_PROC; i++) {
			if (pcb_used[i]) {
				current = &pcb[i];
				// printf("%p\n", current->cp->gpr[2]);
				printf("1\n");
				return current->cp;
			}
		}
	} else {
		for (int i = current - &pcb[0] + 1; i < MAX_NR_PROC; i++) {
			if (pcb_used[i]) {
				current = &pcb[i];
				// printf("%p\n", current->cp->gpr[2]);
				printf("2\n");
				return current->cp;
			}
		}
 		for (int i = 0; i < current - &pcb[0]; i++) {
			if (pcb_used[i]) {
				current = &pcb[i];
				// printf("%p\n", current->cp->gpr[2]);
				printf("3\n");
				return current->cp;
			}
		}
	}
	return NULL;
}
