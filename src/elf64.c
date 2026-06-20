#include "elf64.h"

#include <elf.h>
#include <stdint.h>
#include <string.h>

void setup_ehdr(Elf64_Ehdr *ehdr, uint64_t entry) {
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
  ehdr->e_entry             = entry;
  ehdr->e_phoff             = sizeof(Elf64_Ehdr);
  ehdr->e_ehsize            = sizeof(Elf64_Ehdr);
  ehdr->e_phentsize         = sizeof(Elf64_Phdr);
  ehdr->e_phnum             = 2;
}

void setup_phdr(Elf64_Phdr *phdr, uint32_t flags, uint64_t offset, uint64_t vaddr, uint64_t size) {
  phdr->p_type   = PT_LOAD;
  phdr->p_flags  = flags;
  phdr->p_offset = offset;
  phdr->p_vaddr  = vaddr;
  phdr->p_paddr  = vaddr;
  phdr->p_filesz = size;
  phdr->p_memsz  = size;
  phdr->p_align  = PAGE_SIZE;
}
void write_elf64(FILE *file, uint32_t *text, size_t text_size, uint8_t *data, size_t data_size) {
    uint64_t ehdr_phdr_size = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) * 2;

    uint64_t text_code_vaddr  = LOAD_ADDR + ehdr_phdr_size;

    uint64_t text_seg_offset = 0;
    uint64_t text_seg_vaddr  = LOAD_ADDR;
    uint64_t text_seg_size   = ehdr_phdr_size + text_size;

    uint64_t text_end    = text_seg_offset + text_seg_size;
    uint64_t data_offset = (text_end + PAGE_SIZE - 1) & ~((uint64_t)PAGE_SIZE - 1);
    uint64_t data_vaddr  = LOAD_ADDR + data_offset;

    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));
    setup_ehdr(&ehdr, text_code_vaddr);

    Elf64_Phdr text_phdr;
    memset(&text_phdr, 0, sizeof(text_phdr));
    setup_phdr(&text_phdr, PF_R | PF_X, text_seg_offset, text_seg_vaddr, text_seg_size);

    Elf64_Phdr data_phdr;
    memset(&data_phdr, 0, sizeof(data_phdr));
    setup_phdr(&data_phdr, PF_R, data_offset, data_vaddr, data_size);

    fwrite(&ehdr, sizeof(ehdr), 1, file);
    fwrite(&text_phdr, sizeof(text_phdr), 1, file);
    fwrite(&data_phdr, sizeof(data_phdr), 1, file);
    fwrite(text, text_size, 1, file);

    long current_pos = sizeof(ehdr) + sizeof(text_phdr) + sizeof(data_phdr) + text_size;
    while ((uint64_t)current_pos < data_offset) {
        fputc(0, file);
        current_pos++;
    }

    fwrite(data, data_size, 1, file);
}
