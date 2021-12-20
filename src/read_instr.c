#include <stdio.h>
#include <stdlib.h>
#include "include/headers.h"
#include "include/instr.h"

// #define DEBUG

INS32 *ins32_tab;
int INS32_CNT = 0;

void print_inst(INSTR *inst)
{
    int len = inst->len;

    printf("%8lx:\t", inst->addr);
    for(int i = len / 8 - 1; i >= 0; i--)
        printf("%02x ", inst->inst.readin[i]);
    
    if(len == INS32_LEN)
    {
        if(inst->ptr_32 != NULL)
        {
            printf("\t%d %-10s ", inst->ptr_32->type, inst->ptr_32->name);
            if(inst->rd < 32)
                printf("%s, ", reg[inst->rd]);
            if(inst->rs1 < 32)
                printf("%s", reg[inst->rs1]);
            if(inst->rs2 < 32)
                printf(", %s", reg[inst->rs2]);
            if(inst->rs3 < 32)
                printf(", %s", reg[inst->rs3]);
            if(inst->imm_f)
            {
                switch(inst->ptr_32->type)
                {
                    case 3:
                        printf(", %lx (%d)", (int64_t)inst->addr + (int64_t)inst->imm, inst->imm);
                        break;
                    case 4:
                        printf("%d (0x%x)", inst->imm, inst->imm);
                        break;
                    case 5:
                        printf("%lx (%d)", (int64_t)inst->addr + (int64_t)inst->imm, inst->imm);
                        break;
                    default:
                        printf(", %d (0x%x)", inst->imm, inst->imm);
                        break;
                }
            }
        }
        else
        {
            printf("\tunknown ##################");
        }
    }

#ifdef DEBUG
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
    uint32_t ins_line = inst->inst.inst;
    inst->ptr_32 = NULL;
    // inst->ptr_16 = NULL;
    if(inst->len == INS32_LEN)
    {
        for(int i = 0; i < INS32_CNT; i++)
        {
            uint32_t mk_ins = ins_line & ins32_tab[i].mask;
            if(!(mk_ins ^ ins32_tab[i].func))
                inst->ptr_32 = (ins32_tab + i);
        }

        uint8_t type = inst->ptr_32->type;
        //  filter rd
        switch(type)
        {
            case 0:  case 1:  case 4: case 5:  case 6:
            case 7:  case 8:  case 9: case 10: case 11:
            case 12: case 13: case 14:
                inst->rd = (ins_line & INS32_RDMASK) >> INS32_RDSHFT;
                break;
            default: inst->rd = 32; break;
        }

        //  filter rs1
        switch(type)
        {
            case 0:  case 1:  case 2: case 3:  case 6:
            case 7:  case 9: case 10: case 11: case 12:
            case 13: case 14:
                inst->rs1 = (ins_line & INS32_RS1MASK) >> INS32_RS1SHFT;
                break;
            default: inst->rs1 = 32; break;
        }

        //  filter rs2
        switch(type)
        {
            case 0:  case 2: case 3: case 10: case 11: case 12:
                inst->rs2 = (ins_line & INS32_RS2MASK) >> INS32_RS2SHFT;
                break;
            default: inst->rs2 = 32; break;
        }

        //  filter rs3
        if(type == 11)
            inst->rs3 = (ins_line & INS32_RS3MASK) >> INS32_RS3SHFT;
        else
            inst->rs3 = 32;

        //  filter immediate
        inst->imm_f = 1;
        int32_t s1, s2, s3 = 0;
        switch(type)
        {
            case 1:
                s1 = ins_line >> 20;
                if(ins_line & 0x80000000)
                    s1 = s1 | 0xfffff000;
                inst->imm = s1;
                break;
            case 2:
                s1 = (ins_line >> 7) & 0x1f;
                s2 = (ins_line >> 20) & 0xfe0;
                if(ins_line & 0x80000000)
                    s1 = s1 | 0xfffff000;
                inst->imm = s1 | s2;
                break;
            case 3:
                s1 = (ins_line >> 7) & 0x1e;
                s2 = (ins_line >> 20) & 0x7e0;
                s3 = (ins_line << 4) & 0x800;
                if(ins_line & 0x80000000)
                    s1 = s1 | 0xfffff000;
                // printf("\n%x %x %x\n", s1, s2, s3);
                inst->imm = s1 | s2 | s3;
                break;
            case 4:
                s1 = ins_line >> 12;
                // s1 = ins_line & 0xfffff000;
                inst->imm = s1;
                break;
            case 5:
                s1 = (ins_line >> 20) & 0x7fe;
                s2 = (ins_line >> 9) & 0x800;
                s3 = ins_line & 0xff000;
                // printf("\n%x %x %x\n", s1, s2, s3);
                if(ins_line & 0x80000000)
                    s1 = s1 | 0xfff00000;
                inst->imm = s1 | s2 | s3;
                break;
            case 6:
                s1 = (ins_line >> 20) & 0x1f;
                inst->imm = s1;
                break;
            case 7:
                s1 = (ins_line >> 20) & 0x3f;
                inst->imm = s1;
                break;
            case 9: case 10:
                s1 = (ins_line >> 25) & 0x3;
                inst->imm = s1;
                break;
            default: inst->imm_f = 0; break;
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
        int s;
        s = fscanf(fp, "%hhu,%x,%s\n", &(now->type), &(now->func), now->name);
        if(s != 3)
        {
            printf("invalid format in instr32.csv:");
            printf("%hhu,%x,%s\n", (now->type), (now->func), now->name);
            if(now != ins32_tab)
            {
                printf("previous one is:");
                printf("%hhu,%x,%s\n", ((now-1)->type), ((now-1)->func), (now-1)->name);
            }
            exit(0);
        }
#ifdef DEBUG
        printf("%hhu,%x,%s\n", (now->type), (now->func), now->name);
#endif

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
        // if(stop == 200)
        //     break;
    }
    return(0);
}