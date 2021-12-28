#ifndef __INSTR__
#define __INSTR__
#include <stdint.h>

typedef struct {
    uint32_t mask;
    uint32_t func;
    uint8_t type;
    char name[31];
}INS_ENT;

#define INS16_LEN 16
#define INS32_LEN 32

#define INS32_RDMASK  0x00000F80
#define INS32_RS1MASK 0x000F8000
#define INS32_RS2MASK 0x01F00000
#define INS32_RS3MASK 0xF8000000
#define INS32_RDSHFT  7
#define INS32_RS1SHFT 15
#define INS32_RS2SHFT 20
#define INS32_RS3SHFT 27

#define INS16_FLMASK 0x007C
#define INS16_FSMASK 0x001C
#define INS16_SLMASK 0x0F80
#define INS16_SSMASK 0x0380
#define INS16_FSHFT  2
#define INS16_SSHFT  7

typedef struct {
    union INST {
        uint32_t inst;
        uint8_t  readin[4];
    }inst;
    uint64_t addr;
    int8_t len;

    INS_ENT *ins_ptr;
    int32_t imm;
    uint8_t rd, rs1, rs2, rs3;
}INSTR;

char *reg[] = {"zero", "ra", "sp", "gp",
                 "tp", "t0", "t1", "t2",
                 "s0", "s1", "a0", "a1",
                 "a2", "a3", "a4", "a5",
                 "a6", "a7", "s2", "s3",
                 "s4", "s5", "s6", "s7",
                 "s8", "s9","s10","s11",
                 "t3", "t4", "t5", "t6"};
// f0–7 ft0–7
// f8–9 fs0–1
// f10–11 fa0–1
// f12–17 fa2–7
// f18–27 fs2–11
// f28–31 ft8–11

#endif