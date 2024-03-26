#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
  /* write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  } */
  printf("Hello-test got argument count: %d, args:\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	printf("Hello-test got environment:\n");
	for (int i = 0; envp[i] != NULL; i++) {
		printf("%s\n", envp[i]);
	}

  return 0;
}
