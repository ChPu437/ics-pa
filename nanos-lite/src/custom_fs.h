int fs_open(const char *pathname, int flags, int mode) {
	for (int i = 0; i < file_count; i++) {
		if (!strcmp(pathname, file_table[i].name)) {
				return i;
		}
	}
	assert(0); // file not found
}

size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd) {
	return 0;
}
