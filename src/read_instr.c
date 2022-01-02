#include <stdio.h>
#include <stdlib.h>
#include "include/headers.h"
#include "include/instr.h"

// #define DEBUG

INS_ENT *ins32_tab, *ins16_tab;
int INS32_CNT = 0, INS16_CNT = 0;

void print_inst(INSTR *inst)
{
    int len = inst->len;

    printf("%8lx:\t", inst->addr);
    for(int i = len / 8 - 1; i >= 0; i--)
        printf("%02x ", inst->inst.readin[i]);
    
    if(inst->ins_ptr != NULL)
    {
        char *rd, *rs1, *rs2, *rs3;
        uint32_t imm = inst->imm;
        int64_t jaddr = (int64_t)inst->addr + (int64_t)inst->imm;
        if(inst->rd < 32)
            rd = reg[inst->rd];
        if(inst->rs1 < 32)
            rs1 = reg[inst->rs1];
        if(inst->rs2 < 32)
            rs2 = reg[inst->rs2];
        if(inst->rs3 < 32)
            rs3 = reg[inst->rs3];
        
        if(len == INS32_LEN)
        {
            printf("\t%-10s ", inst->ins_ptr->name);
            switch(inst->ins_ptr->type)
            {
                case 0: case 12:
                    printf("%s, %s, %s", 
                            rd, rs1, rs2);
                    break;
                case 1:
                    printf("%s, %s, %d (0x%x)",
                            rd, rs1, imm, imm);
                    break;
                case 2:
                    printf("%s, %s, %d",
                            rs1, rs2, imm);
                    break;
                case 3:
                    printf("%s, %s, %lx (%d)",
                            rs1, rs2, jaddr, imm);
                    break;
                case 4:
                    printf("%s, %d (0x%x)",
                            rd, imm, imm);
                    break;
                case 5:
                    printf("%s, %lx (0x%x)",
                            rd, jaddr, imm);
                    break;
                case 6: case 7:
                    printf("%s, %s, %d",
                            rd, rs1, imm);
                    break;
                case 8:
                    printf("%s, %d, %d",
                            rd, inst->rs1, imm);
                    break;
                case 9:
                    printf("%s, %s, %x",
                            rd, rs1, imm);
                    break;
                case 10:
                    printf("%s, %s, %s, %x",
                            rd, rs1, rs2, imm);
                    break;
                case 11:
                    printf("%s, %s, %s, %s",
                            rd, rs1, rs2, rs3);
                    break;
                case 13: case 14:
                    printf("%s, %s",
                            rd, rs1);
                    break;
                default:
                    printf("unknown type ########");
                    break;
            }
        }
        else if(len == INS16_LEN)
        {
            printf("\t\t%-10s ", inst->ins_ptr->name);
            switch(inst->ins_ptr->type)
            {
                case 0: case 6: case 8:
                    printf("%s, %d (0x%04x)",
                            rd, imm, imm);
                    break;
                case 1: case 5: case 9: case 10: case 11:
                    printf("%s, %s, %d (0x%04x)",
                            rd, rs1, imm, imm);
                    break;
                case 12:
                    printf("%s, %s, %s",
                            rd, rs1, rs2);
                    break;
                case 13:
                    printf("%lx (0x%04x)",
                            jaddr, imm);
                    break;
                case 14:
                    printf("%s, %lx (0x%04x)",
                            rs1, jaddr, imm);
                    break;
            }
            // rdrrr
        }
    }
    else
    {
        printf("\tunknown ##################");
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
    inst->ins_ptr = NULL;
    if(inst->len == INS32_LEN)
    {
        for(int i = 0; i < INS32_CNT; i++)
        {
            uint32_t mk_ins = ins_line & ins32_tab[i].mask;
            if(!(mk_ins ^ ins32_tab[i].func))
                inst->ins_ptr = (ins32_tab + i);
        }

        if(inst->ins_ptr != NULL)
        {
            uint8_t type = inst->ins_ptr->type;
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
                default: break;
            }

        }
    }
    else if(inst->len == INS16_LEN)
    {
        inst->rs3 = 32;
        for(int i = 0; i < INS16_CNT; i++)
        {
            uint32_t mk_ins = ins_line & ins16_tab[i].mask;
            if(!(mk_ins ^ ins16_tab[i].func))
            {
                inst->ins_ptr = (ins16_tab + i);
            }
        }

        if(inst->ins_ptr != NULL)
        {
            uint8_t type = inst->ins_ptr->type;
            //  filter rd
            switch(type)
            {
                case 0: case 1: case 2:
                    inst->rd = ((ins_line & INS16_FSMASK) >> INS16_FSHFT) + 8;
                    break;
                case 5:  case 6:  case 8: case 15: case 16:
                case 17: case 18: case 20:
                    inst->rd = (ins_line & INS16_SLMASK) >> INS16_SSHFT;
                    break;
                case 9: case 10: case 11: case 12:
                    inst->rd = ((ins_line & INS16_SSMASK) >> INS16_SSHFT) + 8;
                    break;
                    break;
                default: inst->rd = 32; break;
            }

            //  filter rs1
            switch(type)
            {
                case 1: case 2: case 3: case 4: case 14:
                    inst->rs1 = ((ins_line & INS16_SSMASK) >> INS16_SSHFT) + 8;
                    break;
                case 5:  case 9:  case 10: case 11: case 12:
                case 15: case 16: case 21:
                    inst->rs1 = inst->rd;
                    break;
                case 19:
                    inst->rs1 = (ins_line & INS16_SLMASK) >> INS16_SSHFT;
                    break;
                default: inst->rs1 = 32; break;
            }

            //  filter rs2
            switch(type)
            {
                case 3: case 4: case 12:
                    inst->rs2 = ((ins_line & INS16_FSMASK) >> INS16_FSHFT) + 8;
                    break;
                case 20: case 21: case 22: case 23:
                    inst->rs2 = (ins_line & INS16_FLMASK) >> INS16_FSHFT;
                    break;
                default: inst->rs2 = 32; break;
            }

            // rdrrr
            //  filter immediate
            int32_t s1, s2 = 0;
            switch(type)
            {
                case 0:
                    s1 = (ins_line >> 4) & 0x4;
                    s1 = s1 | ((ins_line >> 2) & 0x8);
                    s1 = s1 | ((ins_line >> 7) & 0x30);
                    s1 = s1 | ((ins_line >> 1) & 0x3c0);
                    inst->imm = s1;
                    break;
                case 1: case 3:
                    s1 = (ins_line >> 7) & 0x38;
                    s2 = (ins_line << 1) & 0xc0;
                    inst->imm = s1 | s2;
                    break;
                case 2: case 4:
                    s1 = (ins_line >> 4) & 0x4;
                    s1 = s1 | ((ins_line << 1) & 0x40);
                    s2 = (ins_line >> 7) & 0x38;
                    inst->imm = s1 | s2;
                    break;
                case 5: case 6: case 11:
                    s1 = (ins_line >> 2) & 0x1f;
                    if(ins_line & 0x1000)
                        s1 = s1 | 0xffffffe0;
                    // printf("\n%x %x %x\n", s1, s2, s3);
                    inst->imm = s1;
                    break;
                case 7:
                    s1 = (ins_line >> 2) & 0x10;
                    s1 = s1 | ((ins_line << 3) & 0x20);
                    s1 = s1 | ((ins_line << 1) & 0x40);
                    s1 = s1 | ((ins_line << 4) & 0x180);
                    if(ins_line & 0x1000)
                        s1 = s1 | 0xfffffe00;
                    inst->imm = s1;
                    break;
                case 8:
                    s1 = (ins_line << 10) & 0x1f000;
                    if(ins_line & 0x1000)
                        s1 = s1 | 0xfffe0000;
                    inst->imm = s1 / 4095;
                    break;
                case 9: case 15:
                    s1 = (ins_line >> 2) & 0x1f;
                    s1 = s1 | ((ins_line >> 7) & 0x20);
                    inst->imm = s1;
                    break;
                case 13:
                    s1 = (ins_line >> 2) & 0xe;
                    s1 = s1 | ((ins_line >> 7) & 0x10);
                    s1 = s1 | ((ins_line << 3) & 0x20);
                    s1 = s1 | ((ins_line >> 1) & 0x340);
                    s1 = s1 | ((ins_line << 1) & 0x80);
                    s1 = s1 | ((ins_line << 2) & 0x400);
                    if(ins_line & 0x1000)
                        s1 = s1 | 0xfffff800;
                    inst->imm = s1;
                    break;
                case 14:
                    s1 = (ins_line >> 2) & 0x6;
                    s1 = s1 | ((ins_line >> 7) & 0x18);
                    s1 = s1 | ((ins_line << 3) & 0x20);
                    s1 = s1 | ((ins_line << 1) & 0xc0);
                    if(ins_line & 0x1000)
                        s1 = s1 | 0xffffff00;
                    inst->imm = s1;
                    break;
                case 17:
                    s1 = (ins_line >> 1) & 0x18;
                    s1 = s1 | ((ins_line >> 7) & 0x20);
                    s1 = s1 | ((ins_line << 4) & 0x1c0);
                    inst->imm = s1;
                    break;
                case 18:
                    s1 = (ins_line >> 2) & 0x1c;
                    s1 = s1 | ((ins_line >> 7) & 0x20);
                    s1 = s1 | ((ins_line << 4) & 0xc0);
                    inst->imm = s1;
                    break;
                case 22:
                    s1 = (ins_line >> 7) & 0x38;
                    s1 = s1 | ((ins_line >> 1) & 0x1c0);
                    break;
                case 23:
                    s1 = (ins_line >> 7) & 0x3c;
                    s1 = s1 | ((ins_line >> 1) & 0xc0);
                    break;
                default: break;
            }

        }
    }
    return;
}

#define MAX_LEN 512
void read_ins_file(int type)
{
    FILE *fp;
    if(type == INS32_LEN)
        fp = fopen("./src/instr32.csv", "r");
    else
        fp = fopen("./src/instr16.csv", "r");
    if(fp == NULL)
    {
        printf("failed to open instr%d.csv\n", type);
        exit(0);
    }

    int cnt = 0;
    char line[MAX_LEN];
    INS_ENT *ins_ptr;
    while(fgets(line, MAX_LEN, fp))
        cnt++;
    fseek(fp, 0, SEEK_SET);

    if(type == INS32_LEN)
    {
        INS32_CNT = cnt;
        ins32_tab = calloc(INS32_CNT, sizeof(INS_ENT));
        ins_ptr = ins32_tab;
    }
    else
    {
        INS16_CNT = cnt;
        ins16_tab = calloc(INS16_CNT, sizeof(INS_ENT));
        ins_ptr = ins16_tab;
    }

    for(int i = 0; i < cnt; i++)
    {
        INS_ENT *now = ins_ptr + i;
        int s;
        s = fscanf(fp, "%hhu,%x,%s\n", &(now->type), &(now->func), now->name);
        if(s != 3)
        {
            printf("invalid format in instr%d.csv:%hhu,%x,%s\n",
                    type, (now->type), (now->func), now->name);
            if(i != 0)
            {
                printf("previous one is:%hhu,%x,%s\n",
                        ((now-1)->type), ((now-1)->func), (now-1)->name);
            }
            exit(0);
        }
#ifdef DEBUG
        printf("%hhu,%x,%s\n", (now->type), (now->func), now->name);
#endif

        if(type == INS32_LEN)
        {
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
        else if(type == INS16_LEN)
        {
            switch(now->type)
            {
                case 0:  now->mask = 0xE003; break;
                case 1:  now->mask = 0xE003; break;
                case 2:  now->mask = 0xE003; break;
                case 3:  now->mask = 0xE003; break;
                case 4:  now->mask = 0xE003; break;
                case 5:  now->mask = 0xE003; break;
                case 6:  now->mask = 0xE003; break;
                case 7:  now->mask = 0xE803; break;
                case 8:  now->mask = 0xE003; break;
                case 9:  now->mask = 0xEC03; break;
                case 10: now->mask = 0xFC7F; break;
                case 11: now->mask = 0xEC03; break;
                case 12: now->mask = 0xFC63; break;
                case 13: now->mask = 0xE003; break;
                case 14: now->mask = 0xE003; break;
                case 15: now->mask = 0xE003; break;
                case 16: now->mask = 0xF07F; break;
                case 17: now->mask = 0xE003; break;
                case 18: now->mask = 0xE003; break;
                case 19: now->mask = 0xF07F; break;
                case 20: now->mask = 0xF003; break;
                case 21: now->mask = 0xF003; break;
                case 22: now->mask = 0xE003; break;
                case 23: now->mask = 0xE003; break;
                case 24: now->mask = 0xFFFF; break;
                default: printf("unknown type :%s\n", now->name); break;
            }
        }
    }
    fclose(fp);
    return;
}

int read_instr(SEC_HEADER *header, FILE *fp)
{
    unsigned long saddr = header->header.addr;
    unsigned long size = header->header.size;
    read_ins_file(INS32_LEN);
    read_ins_file(INS16_LEN);

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
        else
        {
            inst.inst.readin[2] = 0;
            inst.inst.readin[3] = 0;
        }

        parse_instr(&inst);

        print_inst(&inst);
        // stop++;
        // if(stop == 20)
        //     break;
    }
    return(0);
}