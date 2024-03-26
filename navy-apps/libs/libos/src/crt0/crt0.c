#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);

extern char **environ;

void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
	/*
	 * String Area
	 * NULL
	 * envp[n - 1]
	 * ...
	 * envp[0]
	 * NULL
	 * argv[n - 1]
	 * ...
	 * argv[0]
	 * argc
	 */
	int argc = (int)*(args++);
	char** argv = empty;
	char** envp = empty;

	/* printf("%d\n", argc);
//	assert(0); // test for argc

	if (argc != 0) {
		// load arguments
		argv = (char**)args;
		while (args != NULL) args++;
	}

	assert((void*)*args == NULL);
	args++; // skip the NULL pointer

	if ((void*)*args != NULL) {
		envp = (char**)args;
	}
	environ = envp; */

  exit(main(argc, argv, envp));
  assert(0); // Should not reach here
}
