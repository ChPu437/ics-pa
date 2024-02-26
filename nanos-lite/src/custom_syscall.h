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

const int32_t _secs_min = 60;
const int32_t _secs_hour = 60 * _secs_min;
const int32_t _secs_day = 24 * _secs_hour;
const int32_t _secs_odd_year = 365 * _secs_day;
const int32_t _secs_even_year = 366 * _secs_day;
struct timeval {
	int32_t tv_sec; // time_t
	int32_t tv_usec; // suseconds_t
};
int32_t sys_gettimeofday(intptr_t tp, intptr_t tzp) {
	AM_TIMER_RTC_T rtc;
	rtc = io_read(AM_TIMER_RTC);
	// this should return 0 on success, whatever other on error
	assert((void*)tzp == NULL);
	struct timeval* _tp = (void*)tp;

	int _cycles = rtc.year / 4;
	_tp->tv_sec = _cycles * (_secs_even_year + 3 * _secs_odd_year) + (rtc.year % 4) * _secs_odd_year;
	_tp->tv_usec = _tp->tv_sec * 1000000; // no 1e6 since no RV32F
	
	return 0;
}
