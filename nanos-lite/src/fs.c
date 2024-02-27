#include <fs.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
extern size_t serial_write(const void* buf, size_t offset, size_t len);
extern size_t events_read(void* buf, size_t offset, size_t len);
extern size_t dispinfo_read(void* buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  // custom VFS file
	{"/dev/events", 0, 0, events_read, invalid_write},
	{"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

int file_count = 0;
int ramdisk_size = 0;
#include "custom_fs.h"

void init_fs() {
  // TODO: initialize the size of /dev/fb
  // ramdisk.c 中有 RAMDISK_SIZE
  // // 由于规定文件紧密排列且不能超过原大小且不能创建新文件，
	file_count = sizeof(file_table) / sizeof(Finfo);
	ramdisk_size = &ramdisk_end - &ramdisk_start;
}
