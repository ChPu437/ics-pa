#include <stdio.h>
#include <memory/paddr.h>

// R/W addr_start(H) addr_end(H) length(xbyte) value(H)

void mtrace_log(paddr_t addr, int len, word_t value, bool is_write) {
	if (is_write)
		printf("MEM: W\t");
	else
		printf("MEM: R\t");
	printf("0x%08x\t0x%08x\t%-dB\t0x%08x\n", addr, addr + 8 * len, len, value);
}
