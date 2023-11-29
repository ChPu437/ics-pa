#include <elf.h>
#include <stdio.h>
#define FSTACK_SIZE 1024;

/*
 * Elf32_Ehdr -> e_shoff // offset for section header
 * section header is an array of Elf32_Shdr
 * for parsing the .strtab, we need to find for Elf32_Shdr->sh_type == SHT_STRTAB
 * then Elf32_Shdr->sh_offset for names
 * in hd: 00 for .
 * strtab holds every entry (other than type SECTION) name in symtab
 * Elf32_Shdr->sh_type == SHT_SYMTAB
 * then Elf32_Shdr->sh_offset for parsing symbol
 *
 * 现在我们就可以把一个给定的地址翻译成函数名了: 
 * 由于函数的范围是互不相交的, 我们可以逐项扫描符号表中Type属性为FUNC的每一个表项, 
 * 检查给出的地址是否落在区间[Value, Value + Size)内, 若是, 则根据表项中的Name属性
 * 在字符串表中找到相应的字符串, 作为函数名返回. 如果没有找到符合要求的符号表表项, 
 * 可以返回字符串"???", 不过这很可能是你的实现错误导致的, 你需要再次检查你的实现.
 * Elf32_Sym->st_info == STT_FUNC
 * Elf32_Sym->st_name 代表这当前symbol的名字在string表中的下标
 * 即寻找strtab[Elf32_Sym->st_name]
 * TODO: 实现功能后也可考虑在寻找对应name前先检查是否超出字符串表范围
 */

/*
 * TODO: 识别程序入口、主函数(判断是否运行到Symbol表中Func项对应地址)
 * TODO: 找到函数对应名字
 * TODO: 识别函数调用
 * 	至于这里为什么示例中add消失了，应该是编译器给他优化了，我们大概不需要管他.
 * TODO: 输出格式(地址、缩进、注释)
 */

extern void* symtab;
extern bool _f_init;

/* static struct g_elf_symtab{
	char* name;
	uint32_t address;
}; */

// TODO: now trace log from buf_log, this depends on ITRACE
// while ftrace dose not need to depends on ITRACE
void ftrace_update(char* log) {
	
}

void ftrace_dump() {
	printf("\n!!!!ftrace: %d\n\n", _f_init);
}

// 想法：读入log用sscanf暴力读出指令地址，匹配指令地址是否为函数入口
// 如果是入口，缩进计数器（栈大小计数器）++，将当前函数压栈，直到运行到当前函数出口点（指令地址回到上一级函数内）弹栈
// 在整个程序运行结束后打印ftrace
