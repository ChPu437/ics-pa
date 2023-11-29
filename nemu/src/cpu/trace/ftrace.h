#include <elf.h>
#include <stdio.h>
#define INST_INDENT 2
#define FSTACK_SIZE 100000
#define FBUF_SIZE FSTACK_SIZE * 2

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

// extern Elf32_Ehdr g_elf_header;
// extern Elf32_Shdr g_section_header[100000];

extern bool g_f_init;
// extern char g_f_strtab[200][200];
extern Elf32_Sym g_f_symtab[100000];
extern int g_cnt_symtab;
extern char g_strtab_str[200 * 200];

static struct {
	uint32_t inst[FSTACK_SIZE]; // 当前指令头
	uint32_t last_addr[FSTACK_SIZE]; // 跳转进来的位置
	int32_t top;
} ftrace_stack;

static struct { // 输出用buf，中间处理过程还是得留个stack
	char inst_buf[FBUF_SIZE][1000];
	int32_t last_addr[FBUF_SIZE];
	int8_t indent[FBUF_SIZE]; // 记录每一个位置的缩进个数
	int32_t cnt;
} ftrace_buf;

// TODO: now trace log from buf_log, this depends on ITRACE
// while ftrace dose not need to depends on ITRACE
static bool isReturn(uint32_t addr) {
	if (ftrace_stack.top <= 2) return 0;
	int last_index = 0;
	int top = ftrace_stack.top;
	for (int i = 0; i < g_cnt_symtab; i++) {
		if (ftrace_stack.inst[top - 2] == g_f_symtab[i].st_value) {
			last_index = i;
			// printf("\n!!!ftrace-last-index: %s\n", g_strtab_str + g_f_symtab[i].st_name);
		}
	}
	if (ftrace_stack.inst[top - 2] <= addr && addr <= ftrace_stack.inst[top - 2] + g_f_symtab[last_index].st_size) {
		return 1;
	}
	return 0;
}

void ftrace_update(char* log) {
	if (!g_f_init) return;
	// TODO: check if current instruction into a new function / return to upper
	static uint32_t last_addr = 0;
	static uint32_t current_addr = 0;
	sscanf(log, "%X:", &current_addr);

	// 处理返回的情况
	// TODO: 无法处理自己调用自己
	if (isReturn(current_addr)) {
		--ftrace_stack.top;
		ftrace_buf.last_addr[ftrace_buf.cnt] = last_addr;
		ftrace_buf.indent[ftrace_buf.cnt] = ftrace_stack.top;
		for (int i = 0; i < g_cnt_symtab; i++) {
			if (g_f_symtab[i].st_value == ftrace_stack.inst[ftrace_stack.top]) {
				sprintf(ftrace_buf.inst_buf[ftrace_buf.cnt], "ret [%s]\n", g_strtab_str + g_f_symtab[i].st_name);
			}
		}
		++ftrace_buf.cnt;
	} else { // 新位置，如果能够查询到则压栈记录
		for (int i = 0; i < g_cnt_symtab; i++) {
			if (g_f_symtab[i].st_value == current_addr) {
				ftrace_buf.last_addr[ftrace_buf.cnt] = last_addr;
				ftrace_buf.indent[ftrace_buf.cnt] = ftrace_stack.top;
				sprintf(ftrace_buf.inst_buf[ftrace_buf.cnt],"call [%s@%X]\n", g_strtab_str + g_f_symtab[i].st_name, current_addr);
				++ftrace_buf.cnt;

				ftrace_stack.inst[ftrace_stack.top] = current_addr;
				ftrace_stack.last_addr[ftrace_stack.top] = last_addr;
				++ftrace_stack.top;
			}
		}
	}

	// 还要记录上一条指令位置(caller_address: N*\t [callee@callee_address])
	last_addr = current_addr;
}

void ftrace_dump() {
	if (!g_f_init) return;
	/* for (int i = 0; i < 20; i++) {
		printf("!!!ftrace: %s\n", g_f_strtab[i]);
	} */
	for (int i = 0; i < g_cnt_symtab; i++) {
		printf("!!!ftrace-symtab-value: %X %u %s\n", g_f_symtab[i].st_value, g_f_symtab[i].st_name, g_strtab_str + g_f_symtab[i].st_name);
	}
	printf("\nFunction call trace:\n");
	for (int i = 0; i < ftrace_buf.cnt; i++) {
		printf("%X: ", ftrace_buf.last_addr[i]);	
		for (int j = 0; j < ftrace_buf.indent[i] * INST_INDENT; j++)
			putchar(' ');
		printf("%s", ftrace_buf.inst_buf[i]);
	}
	// TODO: output
}

// 想法：读入log用sscanf暴力读出指令地址，匹配指令地址是否为函数入口
// 如果是入口，缩进计数器（栈大小计数器）++，将当前函数压栈，直到运行到当前函数出口点
// （指令地址回到上一级函数内 -> 回到返回位置 -> 即上一级函数的下一条语句）弹栈
// 在整个程序运行结束后打印ftrace
