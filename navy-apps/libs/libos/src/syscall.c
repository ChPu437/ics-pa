#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// helper macros
#define _concat(x, y) x ## y
#define concat(x, y) _concat(x, y)
#define _args(n, list) concat(_arg, n) list
#define _arg0(a0, ...) a0
#define _arg1(a0, a1, ...) a1
#define _arg2(a0, a1, a2, ...) a2
#define _arg3(a0, a1, a2, a3, ...) a3
#define _arg4(a0, a1, a2, a3, a4, ...) a4
#define _arg5(a0, a1, a2, a3, a4, a5, ...) a5

// extract an argument from the macro array
#define SYSCALL  _args(0, ARGS_ARRAY)
#define GPR1 _args(1, ARGS_ARRAY)
#define GPR2 _args(2, ARGS_ARRAY)
#define GPR3 _args(3, ARGS_ARRAY)
#define GPR4 _args(4, ARGS_ARRAY)
#define GPRx _args(5, ARGS_ARRAY)

// ISA-depedent definitions
#if defined(__ISA_X86__)
# define ARGS_ARRAY ("int $0x80", "eax", "ebx", "ecx", "edx", "eax")
#elif defined(__ISA_MIPS32__)
# define ARGS_ARRAY ("syscall", "v0", "a0", "a1", "a2", "v0")
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define ARGS_ARRAY ("ecall", "a7", "a0", "a1", "a2", "a0")
#elif defined(__ISA_AM_NATIVE__)
# define ARGS_ARRAY ("call *0x100000", "rdi", "rsi", "rdx", "rcx", "rax")
#elif defined(__ISA_X86_64__)
# define ARGS_ARRAY ("int $0x80", "rdi", "rsi", "rdx", "rcx", "rax")
#elif defined(__ISA_LOONGARCH32R__)
# define ARGS_ARRAY ("syscall 0", "a7", "a0", "a1", "a2", "a0")
#else
#error _syscall_ is not implemented
#endif

intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2) {
  register intptr_t _gpr1 asm (GPR1) = type;
  register intptr_t _gpr2 asm (GPR2) = a0;
  register intptr_t _gpr3 asm (GPR3) = a1;
  register intptr_t _gpr4 asm (GPR4) = a2;
  register intptr_t ret asm (GPRx);
  asm volatile (SYSCALL : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
  return ret;
}

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  _exit(SYS_open);
  return 0;
}

int _write(int fd, void *buf, size_t count) {
	return _syscall_(SYS_write, fd, (intptr_t)buf, count);
	/*
	 * 在Navy的Newlib中, sbrk()最终会调用_sbrk(), 它在navy-apps/libs/libos/src/syscall.c中定义. 
	 * 框架代码让_sbrk()总是返回-1, 表示堆区调整失败, 事实上, 用户程序在第一次调用printf()的时候会尝试通过malloc()
	 * 申请一片缓冲区, 来存放格式化的内容. 若申请失败, 就会逐个字符进行输出. 如果你在Nanos-lite中打开strace, 
	 * 你会发现用户程序通过printf()输出的时候, 确实是逐个字符地调用write()来输出的. 
	 */
}

extern char _end;
void *_sbrk(intptr_t increment) {
	// static uintptr_t program_break_shift = 0;
	static intptr_t program_break = &_end;
	intptr_t last_break = program_break; // 返回旧的program_break
	program_break += increment; // 堆区向高位增长

	char buf[200];
	assert(&_end == program_break);
	sprintf(buf, "%d %d %d!!!end\n", increment, (intptr_t)&_end + increment, ((intptr_t)&_end) + increment);
	_write(1, buf, 100);

	if (!_syscall_(SYS_brk, program_break, 0, 0)) {
		// 正常运行时，brk 返回 0
		return (void*)(last_break);
	}
	else {
		program_break = last_break;
		return (void*)-1;
	}
}  

/* extern char _end;
void *_sbrk(intptr_t increment) {
	// static uintptr_t program_break_shift = 0;
	static uintptr_t program_break_shift = 0;
	uintptr_t last_shift = program_break_shift; // 返回旧的program_break
	program_break_shift += increment; // 堆区向高位增长

	if (!_syscall_(SYS_brk, (uintptr_t)&_end + program_break_shift, 0, 0)) {
		// 正常运行时，brk 返回 0
		return (void*)((uintptr_t)&_end + last_shift);
	}
	else {
		program_break_shift = last_shift;
		return (void*)-1;
	}
} */



/* void *_sbrk(intptr_t increment){
  extern char end;
  static uintptr_t probreak=(uintptr_t)&end;//初始化pb
  uintptr_t probreak_new=probreak+increment;
  int r=_syscall_(SYS_brk,probreak_new,0,0);//系统调用
  if(r==0){//分配成功
    uintptr_t temp=probreak;//旧的pb位置
    probreak=probreak_new;//更新pb
    return (void*)temp;
  }
  return (void *)-1;//分配失败
}  */


int _read(int fd, void *buf, size_t count) {
  _exit(SYS_read);
  return 0;
}

int _close(int fd) {
  _exit(SYS_close);
  return 0;
}

off_t _lseek(int fd, off_t offset, int whence) {
  _exit(SYS_lseek);
  return 0;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
  _exit(SYS_gettimeofday);
  return 0;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  _exit(SYS_execve);
  return 0;
}

// Syscalls below are not used in Nanos-lite.
// But to pass linking, they are defined as dummy functions.

int _fstat(int fd, struct stat *buf) {
  return -1;
}

int _stat(const char *fname, struct stat *buf) {
  assert(0);
  return -1;
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}

pid_t _fork() {
  assert(0);
  return -1;
}

pid_t vfork() {
  assert(0);
  return -1;
}

int _link(const char *d, const char *n) {
  assert(0);
  return -1;
}

int _unlink(const char *n) {
  assert(0);
  return -1;
}

pid_t _wait(int *status) {
  assert(0);
  return -1;
}

clock_t _times(void *buf) {
  assert(0);
  return 0;
}

int pipe(int pipefd[2]) {
  assert(0);
  return -1;
}

int dup(int oldfd) {
  assert(0);
  return -1;
}

int dup2(int oldfd, int newfd) {
  return -1;
}

unsigned int sleep(unsigned int seconds) {
  assert(0);
  return -1;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  assert(0);
  return -1;
}

int symlink(const char *target, const char *linkpath) {
  assert(0);
  return -1;
}

int ioctl(int fd, unsigned long request, ...) {
  return -1;
}
