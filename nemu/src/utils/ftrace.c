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
#define FTRACE_MAX_SYM_SIZE 100000
#define FTRACE_STRTAB_SIZE 200
#define FTRACE_MAX_STR_LENG 200

static Elf32_Ehdr elf_header;
static Elf32_Shdr section_header[FTRACE_MAX_SH_SIZE];
char g_strtab_str[FTRACE_STRTAB_SIZE * FTRACE_MAX_STR_LENG];
// char g_f_strtab[FTRACE_STRTAB_SIZE][FTRACE_MAX_STR_LENG];
static Elf32_Sym symtab_full[FTRACE_MAX_SYM_SIZE];
Elf32_Sym g_f_symtab[FTRACE_MAX_SYM_SIZE];
int g_cnt_symtab = 0;
bool g_f_init = 0;

void init_ftrace(const char *elf_file) {
	if (!elf_file) return; // am mode or none -f specified
	FILE *fp = fopen(elf_file, "r");
	assert(fp); // Check if file can be opened

	bool success;

	fseek(fp, 0, SEEK_SET); // *fp to file start
	success = fread(&elf_header, sizeof(Elf32_Ehdr), 1, fp); // read header to g_f_header
	assert(success);

	fseek(fp, elf_header.e_shoff, SEEK_SET); // *fp to section header start
	// success = fread(g_section_header, g_elf_header.e_shentsize, 1, fp);
	success = fread(section_header, sizeof(Elf32_Shdr), elf_header.e_shnum, fp);
	assert(success);
	// parse string tab 
	// 先写一个strtab的版本，但是可能一个目标文件有多个strtab(后面改成数组)
	int index_strtab = 0; // , cnt_strtab = 0;
	uint64_t size_strtab = 0; // , offset_strtab = 0;
	while(section_header[index_strtab].sh_type != SHT_STRTAB)
		index_strtab++;
	size_strtab = section_header[index_strtab].sh_size;

	fseek(fp, section_header[index_strtab].sh_offset, SEEK_SET); // *fp to strtab start
	success = fread(g_strtab_str, size_strtab, 1, fp);
	assert(success);
	/* while (offset_strtab <= size_strtab) { // read all strings
		sscanf(strtab_str + offset_strtab, "%s", g_f_strtab[cnt_strtab]);
		offset_strtab += strlen(g_f_strtab[cnt_strtab]) + 1;
		++cnt_strtab;
	} */


	// parse symbol tab
	int index_symtab = 0;
	while(section_header[index_symtab].sh_type != SHT_SYMTAB)
		index_symtab++;
	int size_symtab = section_header[index_symtab].sh_size;
	int cnt_symtab_full = size_symtab / section_header[index_symtab].sh_entsize;
	fseek(fp, section_header[index_symtab].sh_offset, SEEK_SET); // *fp to symtab start
	success = fread(symtab_full, section_header[index_symtab].sh_entsize, cnt_symtab_full, fp);
	assert(success);

	// only save FUNC entry for ftrace.h
	for (int i = 0; i < cnt_symtab_full; i++) {
		if (ELF32_ST_TYPE(symtab_full[i].st_info) == STT_FUNC) {
			g_f_symtab[g_cnt_symtab++] = symtab_full[i];
		}
	}

	g_f_init = 1;
	fclose(fp); // we don't want this interfere load_img()
}
