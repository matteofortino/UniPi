#include "../internal.h"
#include "../boot64/elf64.h"

bool find_eh_frame(vaddr elf, vaddr& eh_frame, natq& eh_frame_len)
{
	Elf64_Ehdr *h = ptr_cast<Elf64_Ehdr>(elf);

	for (Elf64_Half i = 0; i < h->e_phnum; i++) {
		Elf64_Phdr *ph = ptr_cast<Elf64_Phdr>(elf + h->e_phoff + h->e_phentsize * i);
		if (ph->p_type == PT_GNU_EH_FRAME) {
			eh_frame = ph->p_vaddr;
			eh_frame_len = ph->p_memsz;
			return true;
		}
	}
	return false;
}
