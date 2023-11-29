/*
 * Initialization for ftrace
 * prepare for a elf_symtab, then we use the tab in src/cpu/trace/ftrace.h
 */

#include <common.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <stdio.h>

#define FTRACE_MAX_SH_SIZE 100000
#define FTRACE_STRTAB_SIZE 200
#define FTRACE_MAX_STR_LENG 200

bool g_f_init = 0;
Elf32_Ehdr g_elf_header;
Elf32_Shdr g_section_header[FTRACE_MAX_SH_SIZE];
char g_strtab_str[FTRACE_STRTAB_SIZE * FTRACE_MAX_STR_LENG];
char g_f_strtab[FTRACE_STRTAB_SIZE][FTRACE_MAX_STR_LENG];

void init_ftrace(const char *elf_file) {
	if (!elf_file) assert(0);
	FILE *fp = fopen(elf_file, "r");
	assert(fp); // Check if file can be opened

	bool success;

	fseek(fp, 0, SEEK_SET); // *fp to file start
	success = fread(&g_elf_header, sizeof(Elf32_Ehdr), 1, fp); // read header to g_f_header
	assert(success);

	fseek(fp, g_elf_header.e_shoff, SEEK_SET); // *fp to section header start
	// success = fread(g_section_header, g_elf_header.e_shentsize, 1, fp);
	success = fread(g_section_header, sizeof(Elf32_Shdr), g_elf_header.e_shnum, fp);
	assert(success);
	// parse string tab 
	// 先写一个strtab的版本，但是可能一个目标文件有多个strtab(后面改成数组)
	int index_strtab = 0, cnt_strtab = 0;
	uint64_t size_strtab = 0, offset_strtab = 0;
	while(g_section_header[index_strtab].sh_type != SHT_STRTAB)
		index_strtab++;
	size_strtab = g_section_header[index_strtab].sh_size;
	fseek(fp, g_section_header[index_strtab].sh_offset, SEEK_SET); // *fp to strtab start
	while (offset_strtab <= size_strtab) { // read all strings
		sscanf(g_strtab_str + offset_strtab, "%s", g_f_strtab[cnt_strtab]);
		offset_strtab += strlen(g_f_strtab[cnt_strtab]) + 1;
		++cnt_strtab;
	}


	// TODO: parse symbol tab
	int index_symtab = 0; // cnt_symtab = 0;
	while(g_section_header[index_symtab].sh_type != SHT_SYMTAB)
		index_symtab++;
	// // map name from strtab
	// // load address(Value) into uint32_t */

	g_f_init = 1;
	fclose(fp); // we don't want this interfere load_img()
}
