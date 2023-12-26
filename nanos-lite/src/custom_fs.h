extern size_t ramdisk_read(void* buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void* buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode) {
	for (int i = 0; i < file_count; i++) {
		if (!strcmp(pathname, file_table[i].name)) {
				return i;
		}
	}
	assert(0); // file not found
}

size_t fs_read(int fd, void *buf, size_t len) {
	assert(fd >= 0 && fd <= file_count);
	if (len + file_table[fd].disk_offset + file_table[fd].open_offset > file_table[fd].size) {
		return -1;
	}
	file_table[fd].open_offset += len;
	ramdisk_read(buf, len + file_table[fd].disk_offset, len);
	return len;
}
size_t fs_write(int fd, const void *buf, size_t len) {
	assert(fd >= 0 && fd <= file_count);
	if (len + file_table[fd].disk_offset + file_table[fd].open_offset > file_table[fd].size) {
		return -1;
	}
	file_table[fd].open_offset += len;
	ramdisk_write(buf, file_table[fd].disk_offset, len);
	return len;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
	assert(fd >= 0 && fd <= file_count);
	return 0;
}

int fs_close(int fd) {
	assert(fd >= 0 && fd <= file_count);
  // 我们并没有真正维护文件是否打开的状态，故关闭文件总是成功
	file_table[fd].open_offset = 0;
	return 0;
}
