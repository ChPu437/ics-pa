/*
 * Initialization for ftrace
 * prepare for a elf_symtab, then we use the tab in src/cpu/trace/ftrace.h
 */

#include <common.h>
#include <string.h>
#include <elf.h>
#include <stdio.h>

#define FTRACE_MAX_SH_SIZE 10000000
#define FTRACE_STRTAB_SIZE 200
#define FTRACE_MAX_STR_LENG 200

bool g_f_init = 0;
Elf32_Ehdr g_elf_header;
static Elf32_Shdr g_section_header[FTRACE_MAX_SH_SIZE];
// static char g_f_strtab[FTRACE_STRTAB_SIZE][FTRACE_MAX_STR_LENG];

void init_ftrace(const char *elf_file) {
	if (!elf_file) assert(0);
	FILE *fp = fopen(elf_file, "r");
	assert(fp); // Check if file can be opened

	bool success = 0;

	fseek(fp, 0, SEEK_SET); // *fp to file start
	success = fread(&g_elf_header, sizeof(Elf32_Ehdr), 1, fp); // read header to g_f_header
	assert(success);

	// TODO: this didn't work (deadloop)
	fseek(fp, g_elf_header.e_shoff, SEEK_SET);
	success = fread(g_section_header, sizeof(Elf32_Shdr), g_elf_header.e_shnum, fp);
	assert(success);
	// TODO: parse string tab 

	// TODO: parse symbol tab
	// // map name from strtab
	// // load address(Value) into uint32_t */

	g_f_init = 1;
	fclose(fp); // we don't want this interfere load_img()
}
