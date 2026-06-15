#include "elf64.h"

#include <elf.h>
#include <stdint.h>
#include <string.h>

void setup_ehdr(Elf64_Ehdr *ehdr, uint64_t load_addr, uint64_t file_offset) {
  ehdr->e_ident[EI_MAG0]    = ELFMAG0;
  ehdr->e_ident[EI_MAG1]    = ELFMAG1;
  ehdr->e_ident[EI_MAG2]    = ELFMAG2;
  ehdr->e_ident[EI_MAG3]    = ELFMAG3;
  ehdr->e_ident[EI_CLASS]   = ELFCLASS64;
  ehdr->e_ident[EI_DATA]    = ELFDATA2LSB;
  ehdr->e_ident[EI_VERSION] = EV_CURRENT;
  ehdr->e_ident[EI_OSABI]   = ELFOSABI_NONE;
  ehdr->e_type              = ET_EXEC;
  ehdr->e_machine           = EM_AARCH64;
  ehdr->e_version           = EV_CURRENT;
  ehdr->e_entry             = load_addr + file_offset;
  ehdr->e_phoff             = sizeof(Elf64_Ehdr);
  ehdr->e_ehsize            = sizeof(Elf64_Ehdr);
  ehdr->e_phentsize         = sizeof(Elf64_Phdr);
  ehdr->e_phnum             = 1;
}

void setup_phdr(Elf64_Phdr *phdr, uint64_t load_addr, uint64_t file_offset, size_t text_size) {
  phdr->p_type   = PT_LOAD;
  phdr->p_flags  = PF_R | PF_X;
  phdr->p_offset = file_offset;
  phdr->p_vaddr  = load_addr + file_offset;
  phdr->p_paddr  = load_addr + file_offset;
  phdr->p_filesz = text_size;
  phdr->p_memsz  = text_size;
  phdr->p_align  = 0x1000;
}

void write_elf64(FILE *file, uint32_t *text, size_t text_size) {
  uint64_t load_addr = 0x400000;
  uint64_t file_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);

  Elf64_Ehdr ehdr;
  memset(&ehdr, 0, sizeof(ehdr));
  setup_ehdr(&ehdr, load_addr, file_offset);
  
  Elf64_Phdr phdr;
  memset(&phdr, 0, sizeof(phdr));
  setup_phdr(&phdr, load_addr, file_offset, text_size);

  fwrite(&ehdr, sizeof(ehdr), 1, file);
  fwrite(&phdr, sizeof(phdr), 1, file);
  fwrite(text, text_size, 1, file);
}
