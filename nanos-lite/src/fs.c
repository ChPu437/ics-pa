#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
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

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

int file_count = 0;
int ramdisk_size = 0;
#include "custom_fs.h"

void init_fs() {
  // TODO: initialize the size of /dev/fb
  // // 由于规定文件紧密排列且不能超过原大小且不能创建新文件，
  // // 可以认为ramdisk大小就是最后一个文件末尾的字节数加1
	file_count = sizeof(file_table) / sizeof(Finfo);
	ramdisk_size = file_table[file_count - 1].size + file_table[file_count - 1].disk_offset;
}
