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
	assert(offset == 0); // 对于stdout和stderr，offset应该始终为0
  for (int i = 0; i < len; i++) {
		putch(*((char*)buf + offset + i));
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	// 读出一条事件信息
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	if (ev.keycode == AM_KEY_NONE) return 0; // 没有按下、松开按键，无有效事件

	if (len) ((char*)buf)[offset + 0] = 'k';
	len--;
	if (ev.keydown) {
		if (len) ((char*)buf)[offset + 1] = 'd';
		len--;
	} else {
		if (len) ((char*)buf)[offset + 1] = 'u';
		len--;
	}
	if (len) ((char*)buf)[offset + 2] = ' ';
	len--;

	offset += 3;
	for (int i = 0; len > 0; i++, len--) {
		((char*)buf)[offset + i] = keyname[ev.keycode][i];
	}

  return 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
