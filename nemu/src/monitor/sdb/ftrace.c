#include <elf.h>
#include <stdio.h>

/*
 * Elf32_Ehdr -> e_shoff // offset for section header
 * section header is an array of Elf32_Shdr
 * for parsing the .strtab, we need to find for Elf32_Shdr->sh_type == SHT_STRTAB
 * then Elf32_Shdr->sh_offset for names
 * in hd: 00 for .
 * strtab holds every entry (other than type SECTION) name in symtab
 * Elf32_Shdr->sh_type == SHT_SYMTAB
 * then Elf32_Shdr->sh_offset for parsing symbol
 */
