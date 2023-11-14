#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define RINGBUF_SIZE 20

static struct {
	char inst_buf[RINGBUF_SIZE][128];
	int8_t cnt;
}iringbuf;

void iringbuf_init() {
	iringbuf.cnt = -1;
}

void iringbuf_update(char* log) {
	if (iringbuf.cnt == RINGBUF_SIZE - 1) {
		iringbuf.cnt = 0;
	} else {
		iringbuf.cnt++;
	}

	strcpy(iringbuf.inst_buf[iringbuf.cnt], log);
}

void iringbuf_dump() {
	printf("Trace of instrucion context(iringbuf):\n");
	if (iringbuf.cnt == -1) {
		printf("[Empty]\n");
	} else {
		for (int i = 0; i < RINGBUF_SIZE; i++) {
			if (i == iringbuf.cnt) {
				printf("----> %s\n", iringbuf.inst_buf[i]);
			} else {
				printf("      %s\n", iringbuf.inst_buf[i]);
			}
		}
	}
}
