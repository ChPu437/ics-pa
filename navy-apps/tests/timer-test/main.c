#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>

/* int checktime(const struct timeval* last_time, struct timeval* curr_time) {
	gettimeofday(curr_time, NULL);
//	printf("%ld\n", curr_time->tv_usec);
	if (curr_time->tv_sec == last_time->tv_sec)
		return curr_time->tv_usec - last_time->tv_usec > 500000;
	return 1;
} */

int main() {
	NDL_Init(0); // don't care about the flag now

	printf("------Start of timer test------\n");
	// printf("size of type \"time_t\" and  \"suseconds_t\": %u, %u\n", sizeof(time_t), sizeof(suseconds_t));

	uint32_t last_time, curr_time;
	last_time = NDL_GetTicks();
	for (int i = 0; i < 50000; i++) {
//		if (checktime(&last_time, &curr_time)) {
		curr_time = NDL_GetTicks();
		if (curr_time - last_time > 500000) {
			printf("Another 0.5s passed!\n");
			last_time = curr_time;
		}
	}

	printf("-------End of timer test-------\n");

  return 0;
}
