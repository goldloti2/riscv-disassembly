#ifndef __HEADERS__
#define __HEADERS__

#include <stdint.h>

// from: https://www.uclibc.org/docs/elf-64-gen.pdf

#define CLASS32 1
#define CLASS64 2

#define DATA2LSB 1
#define DATA2MSB 2

#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_HASH     5
#define SHT_DYNAMIC  6
#define SHT_NOTE     7
#define SHT_NOBITS   8
#define SHT_REL      9
#define SHT_SHLIB   10
#define SHT_DYNSYM  11

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef  int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef  int64_t Elf64_Sxword;


typedef union {
    struct eheader {            // ELF identification
        struct id {
            uint8_t mag[4];     // Magic number, must be "\x7f\ELF"
            uint8_t class;      // File class (1-32bits or 2-64bits)
            uint8_t data;       // Data encoding (1-LSB or 2-MSB)
            uint8_t version;    // File version (1-current)
            uint8_t osabi;      // OS/ABI identification
            uint8_t abiversion; // ABI version
            uint8_t pad[7];     // Padding
        } id;
        Elf64_Half type;        // Object file type
        Elf64_Half machine;     // Machine type
        Elf64_Word version;     // Object file version
        Elf64_Addr entry;       // Entry point address
        Elf64_Off  phoff;       // Program header offset
        Elf64_Off  shoff;       // Section header offset
        Elf64_Word flags;       // Processor-specific flags
        Elf64_Half ehsize;      // ELF header size
        Elf64_Half phentsize;   // Size of program header entry
        Elf64_Half phnum;       // Number of program header entries
        Elf64_Half shentsize;   // Size of section header entry
        Elf64_Half shnum;       // Number of section header entries
        Elf64_Half shstrndx;    // Section name string table index
        } header;
        uint8_t readin[sizeof(struct eheader)];
}ELF_HEADER;



typedef union {
    struct pheader{
        Elf64_Word  type;   // Type of segment
        Elf64_Word  flags;  // Segment attributes
        Elf64_Off   offset; // Offset in file
        Elf64_Addr  vaddr;  // Virtual address in memory
        Elf64_Addr  paddr;  // Reserved
        Elf64_Xword filesz; // Size of segment in file
        Elf64_Xword memsz;  // Size of segment in memory
        Elf64_Xword align;  // Alignment of segment
        } header;
        uint8_t readin[sizeof(struct pheader)];
} PRG_HEADER;



typedef union {
    struct sheader {
        Elf64_Word  name;       // Section name
        Elf64_Word  type;       // Section type
        Elf64_Xword flags;      // Section attributes
        Elf64_Addr  addr;       // Virtual address in memory
        Elf64_Off   offset;     // Offset in file
        Elf64_Xword size;       // Size of section
        Elf64_Word  link;       // Link to other section
        Elf64_Word  info;       // Miscellaneous information
        Elf64_Xword addralign;  // Address alignment boundary
        Elf64_Xword entsize;    // Size of entries, if section has table
        } header;
        uint8_t readin[sizeof(struct sheader)];
}SEC_HEADER;



typedef union {
    struct sym_ent{
        Elf64_Word  st_name;    // Symbol name
        uint8_t     st_info;    // Type and Binding attributes
        uint8_t     st_other;   // Reserved
        Elf64_Half  st_shndx;   // Section table index
        Elf64_Addr  st_value;   // Symbol value
        Elf64_Xword st_size;    // Size of object (e.g., common)
        } entry;
        uint8_t readin[sizeof(struct sym_ent)];
} SYMTAB;

#endif