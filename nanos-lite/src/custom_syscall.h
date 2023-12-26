int32_t sys_yield() {
	yield();
	return 0;
}

void sys_exit() {
	halt(0);	
}

int32_t sys_write(int fd, const char* buf, size_t count) {
	if (fd != 1 && fd != 2) assert(0);
	return count;
}

int32_t sys_brk(intptr_t addr) {
	return 0;
}
