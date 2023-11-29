/*
 * Initialization for ftrace
 * prepare for a elf_symtab, then we use the tab in src/cpu/trace/ftrace.h
 */

#include <common.h>
#include <string.h>
#include <elf.h>
#include <stdio.h>

#define FTRACE_STRTAB_SIZE 128
#define FTRACE_MAX_STR_LENG 256

FILE *g_f_file; // this is only used as a handler to fclose() in ftrace_dump()
bool g_f_init = 0;
static Elf32_Ehdr g_elf_header;
static Elf32_Shdr g_section_header;
// static char g_f_strtab[FTRACE_STRTAB_SIZE][FTRACE_MAX_STR_LENG];

void init_ftrace(const char *elf_file) {
	if (!elf_file) assert(0);
	FILE *fp = fopen(elf_file, "r");
	assert(fp); // Check if file can be opened
	g_f_file = fp; // copy pointer

	bool success = 0;

	fseek(fp, 0, SEEK_SET); // *fp to file start
	success = fread(&g_elf_header, sizeof(Elf32_Ehdr), 1, fp); // read header to g_f_header
	assert(success);

	fseek(fp, g_elf_header.e_shoff, SEEK_SET);
	do { // located to strtab entry
		success = fread(&g_section_header, sizeof(Elf32_Shdr), 1, fp);
		// assert(success);
	} while (g_section_header.sh_type != SHT_STRTAB);
	// TODO: parse string tab 

	fseek(fp, g_elf_header.e_shoff, SEEK_SET);
	do { // located to symtab entry
		success = fread(&g_section_header, sizeof(Elf32_Shdr), 1, fp);
		// assert(success);
	} while (g_section_header.sh_type != SHT_SYMTAB);
	// TODO: parse symbol tab
	// // map name from strtab
	// // load address(Value) into uint32_t */

	g_f_init = 1;
}
