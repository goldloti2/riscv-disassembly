#ifndef __PRT_HDR__
#define __PRT_HDR__

int print_elf(ELF_HEADER EH);
int print_prg(PRG_HEADER *PH, int size);
int print_sec(SEC_HEADER *SH, int size, char *sh_str);
int read_instr(SEC_HEADER *header, FILE *fp);

#endif