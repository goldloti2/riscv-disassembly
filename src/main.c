#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/headers.h"
#include "include/function.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("require a risc-v .o file\n");
        return(0);
    }
    
    // open file
    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL)
    {
        printf("can't find the file: %s\n", argv[1]);
        return(0);
    }

    // read ELF header
    ELF_HEADER EH;
    for(int i = 0; i < sizeof(ELF_HEADER); i++)
        EH.readin[i] = fgetc(fp);
    // if not in ELF format, exit
    if(print_elf(EH) != 0)
        return(0);

    // read program header
    PRG_HEADER *PH = calloc(EH.header.phnum, sizeof(PRG_HEADER));
    for(int i = 0; i < EH.header.phnum; i++)
        for(int j = 0; j < sizeof(PRG_HEADER); j++)
            (PH + i)->readin[j] = fgetc(fp);

    // read section header table
    fseek(fp, EH.header.shoff, SEEK_SET);
    SEC_HEADER *SH = calloc(EH.header.shnum, sizeof(SEC_HEADER));
    SEC_HEADER *sym = NULL, *dynsym = NULL, *strtab = NULL;
    for(int i = 0; i < EH.header.shnum; i++)
    {
        for(int j = 0; j < sizeof(SEC_HEADER); j++)
            (SH + i)->readin[j] = fgetc(fp);
        
        if((SH + i)->header.type == SHT_SYMTAB)
            sym = SH + i;
        if((SH + i)->header.type == SHT_DYNSYM)
            dynsym = SH + i;
        if((SH + i)->header.type == SHT_STRTAB)
            strtab = SH + i;
    }
    // read string table of section header table
    char *sh_str = malloc(strtab->header.size);
    fseek(fp, strtab->header.offset, SEEK_SET);
    for(int i = 0; i < strtab->header.size; i++)
        sh_str[i] = fgetc(fp);

    // read symble table and dynamic symble table, incomplete
    SYMTAB *symtab = NULL, *dyntab = NULL;
    if(sym != NULL)
    {
        int size = sym->header.size;
        symtab = malloc(size);
        fseek(fp, sym->header.offset, SEEK_SET);
        for(int i = 0; i < size / sizeof(SYMTAB); i++)
        {
            for(int j = 0; j < sizeof(SYMTAB); j++)
                (symtab + i)->readin[j] = fgetc(fp);
        }
    }
    if(dynsym != NULL)
    {
        int size = dynsym->header.size;
        dyntab = malloc(size);
        fseek(fp, dynsym->header.offset, SEEK_SET);
        for(int i = 0; i < size / sizeof(SYMTAB); i++)
        {
            for(int j = 0; j < sizeof(SYMTAB); j++)
                (dyntab + i)->readin[j] = fgetc(fp);
        }
    }

    // print headers if argument specified
    if(argc > 2 && strcmp(argv[2], "--header"))
    {
        print_prg(PH, EH.header.phnum);
        print_sec(SH, EH.header.shnum, sh_str);
    }

    // read .text section
    SEC_HEADER *text_sh;
    for(int i = 0; i < EH.header.shnum; i++)
    {
        int name_off = (SH + i)->header.name;
        if(!strcmp((sh_str + name_off), ".text"))
        {
            text_sh = SH + i;
            break;
        }
    }
    // print_sec(text_sh, 1, sh_str);
    read_instr(text_sh, fp);

    fclose(fp);
    return(0);
}
