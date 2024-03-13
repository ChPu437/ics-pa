#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void* buf, size_t len);
extern int fs_close(int fd);
extern int fs_lseek(int fd, size_t offset, int whence);

static uintptr_t loader(PCB *pcb, const char *filename) {
	int fd = fs_open(filename, 0, 0);
  Elf_Ehdr ehdr;
	Elf_Phdr phdr;

  // read elf header
  // ramdisk_read(&ehdr, file_offset, sizeof(Elf_Ehdr));
  assert(~fs_read(fd, &ehdr, sizeof(Elf_Ehdr)));

  // check magic
  assert(ehdr.e_ident[0] == 0x7f);
  assert(ehdr.e_ident[1] == 'E');
  assert(ehdr.e_ident[2] == 'L');
  assert(ehdr.e_ident[3] == 'F');

  // TODO: optional: ISA Check

  // get program header size, count, and 1st entry offset
  // and then get the pointer for pheader entrys
  uint32_t size_phdr = ehdr.e_phentsize;
  uint32_t cnt_phdr = ehdr.e_phnum;
  Elf32_Off off_phdr = ehdr.e_phoff;

  // for each pheader:
  // // if p_type == PT_LOAD:
  // // // read p_offset (文件中的段起点偏移量)
  // // // // p_vaddr (段映射到虚拟内存上的首地址(PA3中我们不区分虚存和物理内存))
  // // // // p_filesz (该段在文件中的字节大小(确定读取终点))
  // // // // p_memsz (该段在虚存中的字节大小)
  // // // 然后将段读入内存，将超出filesz的memsz部分全部设零
	for (int i = 0; i < cnt_phdr; i++) {
		// ramdisk_read(&phdr, off_phdr + size_phdr * i, sizeof(Elf_Phdr));
		assert(~fs_lseek(fd, off_phdr + size_phdr * i, SEEK_SET));
		assert(~fs_read(fd, &phdr, sizeof(Elf_Phdr)));
		if (phdr.p_type != PT_LOAD) continue;
		Elf32_Off off_pent = phdr.p_offset;
		Elf32_Addr vaddr_pent = phdr.p_vaddr; // uint32_t
		uint32_t filesz_pent = phdr.p_filesz;
		uint32_t memsz_pent = phdr.p_memsz;
		// ramdisk_read((void*)(uintptr_t)vaddr_pent, off_pent, filesz_pent);
		assert(~fs_lseek(fd, off_pent, SEEK_SET));
		assert(~fs_read(fd, (void*)(uintptr_t)vaddr_pent, filesz_pent));
		// 03/05/24 Loader写挂了…… 这里memset好像没生效
		// memset((uint32_t*)(uintptr_t)vaddr_pent + filesz_pent, 0, memsz_pent - filesz_pent);
		// printf("%d %d\n", memsz_pent, filesz_pent);
		memset((void*)(vaddr_pent + filesz_pent), 0, memsz_pent - filesz_pent);
		/* for (int i = 0; i < memsz_pent - filesz_pent; i++) {
			printf("%d\n", (*((char*)(vaddr_pent + filesz_pent) + i)));
			assert(*((char*)(vaddr_pent + filesz_pent) + i) == 0);
		} */
		// printf("%x %x %p\n",  filesz_pent, memsz_pent, vaddr_pent);
	}

	fs_close(fd);
  return ehdr.e_entry; // 返回入口点地址
}

void naive_uload(PCB *pcb, const char *filename) {
	assert(filename != NULL);
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
 
void context_uload(PCB *_pcb, const char *filename) {
	assert(filename != NULL);
	uintptr_t entry = loader(NULL, filename);
	_pcb->cp = ucontext(NULL, (Area){(void*)_pcb, (void*)((uintptr_t)_pcb + sizeof(PCB))}, (void*)entry);
	// _pcb->cp->GPRx = (uintptr_t)heap.end;
	_pcb->cp->gpr[2] = (uintptr_t)heap.end;
	Log("Program = \"%s\" registered with Entry = %p\n", filename, entry);
	// ((void(*)())entry) ();
	return;
}

