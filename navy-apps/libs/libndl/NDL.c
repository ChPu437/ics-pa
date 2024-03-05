#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int display_w = 0, display_h = 0;
static struct timeval _NDL_init_time;

void NDL_TODO(char* func) {
	printf("Function \"%s\" not implemnted! Exit now.\n", func);
	assert(0);
}

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
	char ch = fgetc(fp);
	if (ch == (char)255 || ch == '\0') return 0;
	buf[count++] = ch;
	for (; count <= len && buf[count - 1] != '\n'; count++) {
		buf[count] = fgetc(fp);
	}
	buf[count - 1] = '\0'; // currently don't know if we need the LF
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
	if (!(*w) && !(*h)) {
		*w = display_w, *h = display_h;
	}
	screen_w = *w, screen_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	// draw a rectangent and sync the display
	// 使用system open保证即时更新
	int display = open("/dev/fb", O_APPEND);
	for (int i = 0; i < h && i < screen_h; i++) {
		/* for (int j = 0; j < w && j < screen_w; j++) {
			lseek(display, 4 * (i * screen_w + j), SEEK_SET);
			write(display, pixels + i * w + j, 4);
		} */

		// Buffered IO to speed up
		lseek(display, i * screen_w + x, SEEK_SET);
		write(display, pixels + i * w, w);
	}
	close(display);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
	NDL_TODO("NDL_OpenAudio");
}

void NDL_CloseAudio() {
	NDL_TODO("NDL_CloseAudio");
}

int NDL_PlayAudio(void *buf, int len) {
	NDL_TODO("NDL_PlayAudio");
  return 0;
}

int NDL_QueryAudio() {
	NDL_TODO("NDL_QueryAudio");
  return 0;
}

int NDL_Init(uint32_t flags) {
	if (getenv("NWM_APP")) {
    evtdev = 3;
  }

	// get display info
	// // we now don't consider the display size change during process
	FILE* dispinfo = fopen("/proc/dispinfo", "r");
	/* char key = fgetc(dispinfo);
	if (key == 'W') {
		fscanf(dispinfo, "IDTH: %d\n", &screen_w);
		fscanf(dispinfo, "HEIGHT: %d", &screen_h);
	} else {
		fscanf(dispinfo, "EIGHT: %d\n", &screen_h);
		fscanf(dispinfo, "WIDTH: %d", &screen_w);
	} */
	// // hard-written for now
	fscanf(dispinfo, "WIDTH: %d\nHEIGHT: %d", &display_w, &display_h);
	printf("NDL get display info: w=%d, h=%d\n", display_w, display_h);
	fclose(dispinfo);

	// get initialization base time
	gettimeofday(&_NDL_init_time, NULL);
	printf("NDL initiallization success!\n");
  return 0;
}

void NDL_Quit() {
}
