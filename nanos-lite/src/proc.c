#include <proc.h>

#define MAX_NR_PROC 4

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_uload(PCB*, const char*, char* const[], char* const[]);

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
  int j = 1, count = 0;
  Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
  while (1) {
  	count++;
    if (count == 10000) {
			j++;
    	count = 0;
    	Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    }

    yield();
  }
}


void wrapper_uload(PCB* _pcb, const char* filename, char* const argv[], char* const envp[]) {
	context_uload(_pcb, filename, argv, envp);
	for (int i = 0; i < MAX_NR_PROC; i++) {
		if (_pcb == &pcb[i]) {
			pcb_used[i] = 1;
			break;
		}
	}
}

void init_proc() {
  Log("Initializing processes...");
  context_kload(&pcb[0], hello_fun, (void*)1L);
  // context_kload(&pcb[1], hello_fun, (void*)9L);
  // wrapper_uload(&pcb[1], "/bin/nslider", NULL, NULL);
  char* test_arg[] = {"aaa", "bbb", "123"};
  char* test_env[] = {"PATH=/bin", "CFLAGS=-O2"};
  wrapper_uload(&pcb[1], "/bin/nslider", test_arg, test_env);

  switch_boot_pcb();

  // naive_uload(NULL, "/bin/bird");
}

Context* schedule(Context *prev) {
  current->cp = prev;
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
