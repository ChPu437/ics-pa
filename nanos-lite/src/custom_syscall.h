extern size_t fs_open(const char*, int, int);
extern size_t fs_read(int, char*, size_t);
extern size_t fs_write(int, const char*, size_t);
extern int fs_close(int);
extern size_t fs_lseek(int, size_t, int);

int32_t sys_yield() {
	yield();
	return 0;
}

void sys_exit() {
	halt(0);	
}

int32_t sys_open(const char *pathname, int flags, int mode) {
	return fs_open(pathname, flags, mode);
}

int32_t sys_read(int fd, char* buf, size_t count) {
	return fs_read(fd, buf, count);
}

int32_t sys_write(int fd, const char* buf, size_t count) {
	return fs_write(fd, buf, count);
/*	if (fd != 1 && fd != 2) {
		assert(0);
	}
	for (int i = 0; i < count; i++) {
		putch(*(buf + i));
	}
	return count; */
}

int32_t sys_close(int fd) {
	return fs_close(fd);
}

int32_t sys_lseek(int fd, size_t offset, int whence) {
	return fs_lseek(fd, offset, whence);
}

int32_t sys_brk(intptr_t addr) {
	return 0;
}
