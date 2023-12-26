extern size_t ramdisk_read(void* buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void* buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode) {
	for (int i = 0; i < file_count; i++) {
		if (!strcmp(pathname, file_table[i].name)) {
				file_table[i].open_offset = 0; // so we surely don't want multiple open request on one file
				return i;
		}
	}
	assert(0); // file not found
}

size_t fs_read(int fd, void *buf, size_t len) {
	assert(fd >= 0 && fd <= file_count);

		printf("$$$%d %d %d\n", len, file_table[fd].open_offset, file_table[fd].size);
	if (len + file_table[fd].open_offset - 1 > file_table[fd].size)
		assert(0);
	ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	file_table[fd].open_offset += len;
	return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
	assert(fd >= 0 && fd <= file_count);

	if (fd == 1 || fd == 2) {
		for (int i = 0; i < len; i++) 
			putch(*((char*)buf + i));
	} else {
		if (len + file_table[fd].open_offset - 1 > file_table[fd].size) 
			return -1;
		ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
	}
	return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
	assert(fd >= 0 && fd <= file_count);
	switch(whence) {
		case SEEK_SET:
			// if (offset >= file_table[fd].size) return -1;
			// lseek允许offset大于文件大小，这个判断应该发生在rw过程中
			file_table[fd].open_offset = offset;
			break;
		case SEEK_CUR:
			// if (offset + file_table[fd].open_offset >= file_table[fd].size)
			// 	return -1;
			file_table[fd].open_offset += offset;
			break;
		case SEEK_END:
			// if (offset != 0) return -1;
			file_table[fd].open_offset = file_table[fd].size;
			break;
		default:
			assert(0);
	}
	return file_table[fd].open_offset;
}

int fs_close(int fd) {
	assert(fd >= 0 && fd <= file_count);
  // 我们并没有真正维护文件是否打开的状态，故关闭文件总是成功
	file_table[fd].open_offset = 0;
	return 0;
}
