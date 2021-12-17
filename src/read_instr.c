#include <stdio.h>
#include <stdlib.h>
#include "include/headers.h"
#include "include/instr.h"

// #define PRINT_INSTR_BIT

INS32 *ins32_tab;
int INS32_CNT = 0;

void print_inst(INSTR *inst)
{
    printf("%p\n", inst);
    int len = inst->len;

    printf("%8lx:\t", inst->addr);
    for(int i = len / 8 - 1; i >= 0; i--)
        printf("%02x ", inst->inst.readin[i]);
    
    if(len == INS32_LEN)
    {
        printf("\t%-10s `", inst->ptr_32->name);
    }

#ifdef PRINT_INSTR_BIT
    printf("\n\t");
    int sc = 0;
    uint32_t p = 1 << (len -1);
    for(int i = len - 1; i >= 0; i--)
    {
        printf("%d", (inst->inst.inst & p)?1:0);
        p = p >> 1;
        sc++;
        if(!(sc % 8))
            printf(" ");
    }
#endif

    printf("\n");
    return;
}

void parse_instr(INSTR *inst)
{
    int find_op = 0;
    inst->ptr_32 = NULL;
    // inst->ptr_16 = NULL;
    if(inst->len == INS32_LEN)
    {
        for(int i = 0; i < INS32_CNT; i++)
        {
            uint32_t mk_ins = inst->inst.inst & ins32_tab[i].mask;
            if(!(mk_ins ^ ins32_tab[i].func))
            {
                // printf("%s\n", ins32_tab[i].name);
                inst->ptr_32 = (ins32_tab + i);
            }
        }
    }
    else if(inst->len == INS16_LEN)
    {
        ;
    }
    return;
}

#define MAX_LEN 512
void read_ins32()
{
    FILE *fp = fopen("./src/instr32.csv", "r");
    if(fp == NULL)
    {
        printf("failed to open instr32.csv\n");
        exit(0);
    }

    char line[MAX_LEN];
    while(fgets(line, MAX_LEN, fp))
        INS32_CNT++;
    fseek(fp, 0, SEEK_SET);

    ins32_tab = calloc(INS32_CNT, sizeof(INS32));
    for(int i = 0; i < INS32_CNT; i++)
    {
        INS32 *now = ins32_tab + i;
        fscanf(fp, "%hhu,%x,%s\n", &(now->type), &(now->func), now->name);
        // printf("`%hhu,%x,%s`\n", (now->type), (now->func), now->name);

        switch(now->type)
        {
            case 0:  now->mask = 0xFE00707F; break;
            case 1:  now->mask = 0x0000707F; break;
            case 2:  now->mask = 0x0000707F; break;
            case 3:  now->mask = 0x0000707F; break;
            case 4:  now->mask = 0x0000007F; break;
            case 5:  now->mask = 0x0000007F; break;
            case 6:  now->mask = 0xFE00707F; break;
            case 7:  now->mask = 0xFC00707F; break;
            case 8:  now->mask = 0x0000707F; break;
            case 9:  now->mask = 0xF9F0707F; break;
            case 10: now->mask = 0xF800007F; break;
            case 11: now->mask = 0x0600707F; break;
            case 12: now->mask = 0xFE00007F; break;
            case 13: now->mask = 0xFFF0007F; break;
            case 14: now->mask = 0xFFF0707F; break;
            case 15: now->mask = 0xF00FFFFF; break;
            case 16: now->mask = 0xFFFFFFFF; break;
            default: printf("unknown type :%s\n", now->name); break;
        }
    }
    fclose(fp);
    return;
}

int read_instr(SEC_HEADER *header, FILE *fp)
{
    unsigned long saddr = header->header.addr;
    unsigned long size = header->header.size;
    read_ins32();

    int stop = 0;

    fseek(fp, header->header.offset, SEEK_SET);
    for(unsigned long addr = 0; addr < size; addr += 2)
    {
        INSTR inst;
        inst.addr = saddr + addr;
        inst.len = INS16_LEN;
        for(int i = 0; i < 2; i++)
            inst.inst.readin[i] = fgetc(fp);
        if((inst.inst.readin[0] & 0b11) == 0b11)
        {
            for(int i = 2; i < 4; i++)
                inst.inst.readin[i] = fgetc(fp);
            inst.len = INS32_LEN;
            addr += 2;
        }

        parse_instr(&inst);

        print_inst(&inst);
        // stop++;
        // if(stop == 50)
        //     break;
    }
    return(0);
}