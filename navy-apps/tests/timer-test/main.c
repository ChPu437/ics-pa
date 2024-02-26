#include <stdio.h>
#include <sys/time.h>

int checktime(const struct timeval* last_time, struct timeval* curr_time) {
	gettimeofday(curr_time, NULL);
	if (curr_time->tv_sec == last_time->tv_sec)
		return curr_time->tv_usec - last_time->tv_usec > 500000;
	return 1;
}

int main() {
	printf("------Start of timer test------\n");
	printf("size of type \"time_t\" and  \"suseconds_t:\" %u, %u\n", sizeof(time_t), sizeof(suseconds_t));

	struct timeval last_time, curr_time;
	gettimeofday(&last_time, NULL);
	printf("bao\n");
	for (int i = 0; i < 500; i++) {
		gettimeofday(&curr_time, NULL);
		if (checktime(&last_time, &curr_time)) {
			printf("Another 0.5s passed!\n");
			last_time = curr_time;
		}
	}

	printf("-------End of timer test-------\n");

  return 0;
}
