int32_t sys_yield() {
	yield();
	return 0;
}

void sys_exit() {
	halt(0);	
}

int32_t sys_open(const char *pathname, int flags, int mode) {
#ifdef HAS_NAVY
	return fs_open(pathname, flags, mode);
#else
	return -1;
#endif
}

int32_t sys_read(int fd, char* buf, size_t count) {
#ifdef HAS_NAVY
	return fs_read(fd, buf, count);
#else
	return -1;
#endif
}

int32_t sys_write(int fd, const char* buf, size_t count) {
#ifdef HAS_NAVY
	return fs_write(fd, buf, count);
#else
	if (fd != 1 && fd != 2) {
		assert(0);
	}
	for (int i = 0; i < count; i++) {
		putch(*(buf + i));
	}
	return count;
#endif
}

int32_t sys_close(int fd) {
#ifdef HAS_NAVY
	return fs_close(fd);
#else
	return -1;
#endif
}

int32_t sys_lseek(int fd, size_t offset, int whence) {
#ifdef HAS_NAVY
	return fs_lseek(fd, offset, whence);
#else 
	return -1;
#endif
}

int32_t sys_brk(intptr_t addr) {
	return 0;
}
