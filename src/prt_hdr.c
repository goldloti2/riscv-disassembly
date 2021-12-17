#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/headers.h"

void print_byte(unsigned char read[], int size)
{
#ifdef PRINT_BYTE
    for(int i = 0; i < size; i += 2)
        printf("%02x%02x ", read[i], read[i+1]);
    printf("\n");
#else
    return;
#endif
}

int print_elf(ELF_HEADER EH)
{
    printf("ELF Header content:\n\nfile format:\t");
    if(!(EH.header.id.mag[0] == '\x7f' && EH.header.id.mag[1] == 'E' && EH.header.id.mag[2] == 'L' && EH.header.id.mag[3] == 'F'))
    {
        printf("unknown\n");
        return(-1);
    }
    printf("ELF");
    
    switch(EH.header.id.class)
    {
        case CLASS32: printf("32-"); break;
        case CLASS64: printf("64-"); break;
    }
    switch(EH.header.id.data)
    {
        case DATA2LSB: printf("little\n"); break;
        case DATA2MSB: printf("big\n"); break;
    }
    
    printf("file type:\t");
    switch(EH.header.type)
    {
        case ET_NONE: printf("No file type\n"); break;
        case ET_REL:  printf("Relocatable object file\n"); break;
        case ET_EXEC: printf("Executable file\n"); break;
        case ET_DYN:  printf("Shared object file\n"); break;
        case ET_CORE: printf("Core file\n"); break;
        default: printf("0x%04x\n", EH.header.type); break;
    }
    
    printf("machine type:\t0x%04x\n", EH.header.machine);
    printf("version:\t%d\n", EH.header.version);
    printf("entry addr:            \t0x%016lx\n", EH.header.entry);
    printf("program header offset:\t0x%016lx\n", EH.header.phoff);
    printf("section header offset:\t0x%016lx\n", EH.header.shoff);
    printf("flags:\t0x%x\n", EH.header.flags);
    printf("ELF header size:    \t%d (bytes)\n", EH.header.ehsize);
    printf("program header size:\t%d (bytes)\n", EH.header.phentsize);
    printf("program header num:\t%d\n", EH.header.phnum);
    printf("section header size:\t%d (bytes)\n", EH.header.shentsize);
    printf("section header num:\t%d\n", EH.header.shnum);
    printf("section name string table index: %d\n", EH.header.shstrndx);

    print_byte(EH.readin, sizeof(EH));

    printf("\n----------------------------------------------\n\n");

    return(0);
}

int print_prg(PRG_HEADER *PH, int size)
{
    PRG_HEADER *ph;
    printf("Program Header content:");
    for(int i = 0; i < size; i++)
    {
        ph = PH + i;
        printf("\n#%02d\ntype of segment:\t", i);
        switch(ph->header.type)
        {
            case PT_NULL:    printf("Unused entry\n"); break;
            case PT_LOAD:    printf("Loadable segment\n"); break;
            case PT_DYNAMIC: printf("Dynamic linking tables\n"); break;
            case PT_INTERP:  printf("Program interpreter path name\n"); break;
            case PT_NOTE:    printf("Note sections\n"); break;
            case PT_SHLIB:   printf("Reserved\n"); break;
            case PT_PHDR:    printf("Program header table\n"); break;
            default: printf("0x%08x\n", ph->header.type); break;
        }
        
        printf("Attributes flags:\t");
        char prm[4] = {'-', '-', '-', '\0'};
        if(ph->header.flags & 0x1)
            prm[2] = 'x';
        if(ph->header.flags & 0x2)
            prm[1] = 'w';
        if(ph->header.flags & 0x4)
            prm[0] = 'r';
        printf("%s, 0x%08x\n", prm, ph->header.flags);

        printf("Offset in file:\t0x%016lx\n", ph->header.offset);
        printf("Virtual addr:  \t0x%016lx\n", ph->header.vaddr);
        printf("physical addr: \t0x%016lx\n", ph->header.paddr);
        printf("Size in file:  \t0x%016lx\n", ph->header.filesz);
        printf("Size in memory:\t0x%016lx\n", ph->header.memsz);
        printf("Alignment     :\t0x%016lx\n", ph->header.align);

        print_byte(ph->readin, sizeof(ph));
    }

    printf("\n----------------------------------------------\n\n");

    return(0);
}

int print_sec(SEC_HEADER *SH, int size, char *sh_str)
{
    SEC_HEADER *sh;
    printf("Section Table content:");
    for(int i = 0; i < size; i++)
    {
        sh = SH + i;
        printf("\n#%02d\nname:\t%s\nSection type:\t", i, sh_str + sh->header.name);
        switch(sh->header.type)
        {
            case SHT_NULL:     printf("NULL\n"); break;
            case SHT_PROGBITS: printf("PROGBITS\n"); break;
            case SHT_SYMTAB:   printf("SYMTAB\n"); break;
            case SHT_STRTAB:   printf("STRTAB\n"); break;
            case SHT_RELA:     printf("RELA\n"); break;
            case SHT_HASH:     printf("HASH\n"); break;
            case SHT_DYNAMIC:  printf("DYNAMIC\n"); break;
            case SHT_NOTE:     printf("NOTE\n"); break;
            case SHT_NOBITS:   printf("NOBITS\n"); break;
            case SHT_REL:      printf("REL\n"); break;
            case SHT_SHLIB:    printf("Reserved\n"); break;
            case SHT_DYNSYM:   printf("DYNSYM\n"); break;
            default: printf("0x%08x\n", sh->header.type); break;
        }
        printf("Section flags:\t");
        char prm[4] = {'-', '-', '-', '\0'};
        if(sh->header.flags & 0x1)
            prm[0] = 'W';
        if(sh->header.flags & 0x2)
            prm[1] = 'A';
        if(sh->header.flags & 0x4)
            prm[2] = 'X';
        printf("%s, 0x%016lx\n", prm, sh->header.flags);
        printf("addr:      \t0x%016lx\n", sh->header.addr);
        printf("offset:   \t0x%016lx\n", sh->header.offset);
        printf("size:      \t0x%016lx\n", sh->header.size);
        printf("entry size:\t0x%016lx\n", sh->header.entsize);
        printf("link:\t%d\n", sh->header.link);
        printf("info:\t%d\n", sh->header.info);
        printf("align:\t%ld\n", sh->header.addralign);

        print_byte(sh->readin, sizeof(sh));
    }

    printf("\n----------------------------------------------\n\n");

    return(0);
}