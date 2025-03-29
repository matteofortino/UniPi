/// @cond INTERNAL
#define ET_NONE		0
#define	ET_REL		1
#define	ET_EXEC		2
#define	ET_DYN		3
#define	ET_CORE		4
#define	ET_LOPROC	0xff00
#define	ET_HIPROC	0xffff

#define EM_NONE		0
#define	EM_M32		1
#define	EM_SPARC	2
#define EM_386		3
#define EM_68K		4
#define EM_88K		5
#define EM_860		7
#define EM_MIPS		8
#define EM_AMD64	62


#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION 	6
#define EI_PAD		7
#define EI_NIDENT 	16

#define ELFMAG0		0x7f
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'

#define ELFCLASSNONE 	0
#define ELFCLASS32	1
#define ELFCLASS64	2

#define ELFDATANONE	0
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define PF_R            0x4
#define PF_W            0x2
#define PF_X            0x1
#define	PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff
#define PT_GNU_EH_FRAME	0x6474e550
#define SHT_NULL                        0
#define SHT_PROGBITS                    1
#define SHT_SYMTAB                      2
#define SHT_STRTAB                      3
#define SHT_RELA                        4
#define SHT_HASH                        5
#define SHT_DYNAMIC                     6
#define SHT_NOTE                        7
#define SHT_NOBITS                      8
#define SHT_REL                         9
#define SHT_SHLIB                      10
#define SHT_DYNSYM                     11
#define SHT_LOPROC             0x70000000
#define SHT_HIPROC             0x7fffffff
#define SHT_LOUSER             0x80000000
#define SHT_HIUSER             0xffffffff


#define SHF_WRITE                  0x1
#define SHF_ALLOC                  0x2
#define SHF_EXECINSTR              0x4
#define SHF_MASKPROC               0xf0000000

typedef natw   Elf64_Half;
typedef natl   Elf64_Word;
typedef natq   Elf64_Off;
typedef natq   Elf64_Addr;
typedef natq   Elf64_XWord;

typedef struct {
        unsigned char e_ident[EI_NIDENT];
        Elf64_Half    e_type;
        Elf64_Half    e_machine;
        Elf64_Word    e_version;
        Elf64_Addr    e_entry;
        Elf64_Off     e_phoff;
        Elf64_Off     e_shoff;
        Elf64_Word    e_flags;
        Elf64_Half    e_ehsize;
        Elf64_Half    e_phentsize;
        Elf64_Half    e_phnum;
        Elf64_Half    e_shentsize;
        Elf64_Half    e_shnum;
        Elf64_Half    e_shstrndx;
} Elf64_Ehdr;

typedef struct {
        Elf64_Word p_type;
	Elf64_Word p_flags;
        Elf64_Off  p_offset;
        Elf64_Addr p_vaddr;
        Elf64_Addr p_paddr;
        Elf64_XWord p_filesz;
        Elf64_XWord p_memsz;
        Elf64_XWord p_align;
} Elf64_Phdr;

typedef struct {
        Elf64_Word	sh_name;
        Elf64_Word	sh_type;
        Elf64_XWord	sh_flags;
        Elf64_Addr	sh_addr;
        Elf64_Off	sh_offset;
        Elf64_XWord	sh_size;
        Elf64_Word	sh_link;
        Elf64_Word	sh_info;
        Elf64_XWord	sh_addralign;
        Elf64_XWord	sh_entsize;
} Elf64_Shdr;
/// @endcond
