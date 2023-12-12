#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr* ehdr = NULL;
	Elf_Phdr* phdr = NULL;

  // read elf header
  ramdisk_read((void*)ehdr, 0, sizeof(Elf_Ehdr));
  assert(ehdr);

  // TODO: check magic

  // get program header size, count, and 1st entry offset
  // and then get the pointer for pheader entrys
  uint32_t size_phdr = ehdr->e_phentsize;
  uint32_t cnt_phdr = ehdr->e_phnum;
  Elf32_Off off_phdr = ehdr->e_phoff;

  // for each pheader:
  // // if p_type == PT_LOAD:
  // // // read p_offset (文件中的段起点偏移量)
  // // // // p_vaddr (段映射到虚拟内存上的首地址(PA3中我们不区分虚存和物理内存))
  // // // // p_filesz (该段在文件中的字节大小(确定读取终点))
  // // // // p_memsz (该段在虚存中的字节大小)
  // // // 然后将段读入内存，将超出filesz的memsz部分全部设零
  uintptr_t entry_addr = 0xffffffff;
	for (int i = 0; i < cnt_phdr; i++) {
		ramdisk_read((void*)phdr, off_phdr + size_phdr * i, sizeof(Elf_Phdr));
		assert(phdr);
		if (phdr->p_type != PT_LOAD) continue;
		Elf32_Off off_pent = phdr->p_offset;
		Elf32_Addr vaddr_pent = phdr->p_vaddr;
		uint32_t filesz_pent = phdr->p_filesz;
		uint32_t memsz_pent = phdr->p_memsz;
		ramdisk_read((void*)vaddr_pent, off_pent, filesz_pent);
		memset((uint32_t*)vaddr_pent + filesz_pent, 0, memsz_pent - filesz_pent);
		entry_addr = entry_addr > vaddr_pent ? vaddr_pent : entry_addr;
	}

	assert(entry_addr != 0xffffffff);
  return entry_addr; // 返回入口点地址
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

	assert(0);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

