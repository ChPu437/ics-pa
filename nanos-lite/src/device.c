#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	assert(offset == 0); // 对于stdout和stderr，offset应该始终为0, 因为并不支持lseek
  for (int i = 0; i < len; i++) {
		putch(*((char*)buf + offset + i));
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	// Log("events_read() triggered! offset: %d, len: %d", offset, len);
	int count = 0;
	// 读出一条事件信息
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	if (ev.keycode == AM_KEY_NONE) return 0; // 无有效事件

	if (len) ((char*)buf)[offset + count++] = 'k';
	len--;
	if (ev.keydown) {
		if (len) ((char*)buf)[offset + count++] = 'd';
		len--;
	} else {
		if (len) ((char*)buf)[offset + count++] = 'u';
		len--;
	}
	if (len) ((char*)buf)[offset + count++] = ' ';
	len--;

	for (int i = 0; len > 0 && keyname[ev.keycode][i] != '\0'; i++, len--) {
		((char*)buf)[offset + count++] = keyname[ev.keycode][i];
	}
	if (len) ((char*)buf)[offset + count++] = '\n';

  return count;
}

static int _get_width(int x) {
	int count = 1;
	while(x > 10) {
		x /= 10, count++;
	}
	return count;
}
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	assert(offset == 0); // offset应该始终为0, 因为并不支持lseek
  int screen_width = io_read(AM_GPU_CONFIG).width;
  int screen_height = io_read(AM_GPU_CONFIG).height;
  Log("dispinfo get screen info w=%d, h=%d", screen_width, screen_height);

	char* _buf = (char*)buf;
	int len_width = _get_width(screen_width);
	int len_height = _get_width(screen_height);
	int totlen = 7 + len_width + 1 + 8 + len_height + 1;
		// WIDTH: screen_width\nHEIGHT: screen_height\0
	if (len > totlen)
		sprintf(_buf + offset, "WIDTH: %d\nHEIGHT: %d", screen_width, screen_height);

  return totlen;
}

static int _mod(const int x, const int y) {
	return x - (x / y) * y;
}
size_t fb_write(const void *buf, size_t offset, size_t len) {
	// 显示设备需要支持lseek
	Log("fb write triggered, offset = %d", offset);

	uint32_t pixel = *(uint32_t*)buf;
  int screen_width = io_read(AM_GPU_CONFIG).width;
  // int screen_height = io_read(AM_GPU_CONFIG).height;
  int x = _mod(offset, screen_width);
  int y = offset / screen_width;
	io_write(AM_GPU_FBDRAW, x, y, &pixel, 1, 1, 1);
	
	Log("printing %x to display with coordinate %d, %d.", pixel, x, y);

  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
