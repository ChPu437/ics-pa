#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
struct timeval _NDL_init_time;

uint32_t NDL_GetTicks() {
	struct timeval curr_time; 
	gettimeofday(&curr_time, NULL);
  return curr_time.tv_usec - _NDL_init_time.tv_usec;
}

int NDL_PollEvent(char *buf, int len) {
	/*
	 https://bytes.com/topic/c/answers/218856-fopen-vs-open

	 Buffering is not the issue: files accessed with open() and friends are
	 buffered, too.

	 The real issue, apart from portability (open() and friends are actually
	 *very* portable, even if the standard doesn't guarantee it), is that,
	 on certain platforms, read() and write() are OS primitives and, therefore,
	 much more expensive than ordinary library calls. No big deal when used
	 with large data chunks, but using read() instead of getc() and write()
	 instead of putc() may destroy the program's performance. That's why
	 the <stdio.h> I/O routines add one *additional* layer of buffering.
	 */
	FILE* fp = fopen("/dev/events", "r");

	int count = 0;
	if (buf[count] == (char)255) return 0;
	do {
		buf[count] = fgetc(fp);
		// printf("char: %d\n", buf[count]);
	} while(count++ < len);
	fclose(fp);

  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
	if (getenv("NWM_APP")) {
    evtdev = 3;
  }

	gettimeofday(&_NDL_init_time, NULL);
  return 0;
}

void NDL_Quit() {
}
