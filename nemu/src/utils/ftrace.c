/*
 * Initialization for ftrace
 * prepare for a elf_symtab, then we use the tab in src/cpu/trace/ftrace.h
 */

#include <common.h>
#include <elf.h>

#include <stdio.h>
bool _f_init = 0;
void init_ftrace(const char *elf_file) {
	_f_init = 1;
}
