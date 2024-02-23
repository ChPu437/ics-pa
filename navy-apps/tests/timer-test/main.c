#include <stdio.h>
#include <time.h>

int checktime(const struct timeval* last_time, struct timeval* curr_time) {
	gettimeofday(curr_time, NULL);
	if (curr_time->tv_sec == last_time->tv_sec)
		return curr_time->tv_usec - last_time->tv_usec > 500000;
	return 1;
}

int main() {
	printf("------Start of timer test------\n");

	struct timeval last_time, curr_time;
	gettimeofday(&last_time, NULL);
	for (int i = 0; i < 50000; i++) {
		gettimeofday(&curr_time, NULL);
		if (checktime(&last_time, &curr_time)) {
			printf("Another 0.5s passed!\n");
			last_time = curr_time;
		}
	}

	printf("-------End of timer test-------\n");

  return 0;
}
