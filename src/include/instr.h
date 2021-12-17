#ifndef __INSTR__
#define __INSTR__
#include <stdint.h>

typedef struct {
    uint32_t mask;
    uint32_t func;
    uint8_t type;
    char name[31];
}INS32;

#define INS16_LEN 16
#define INS32_LEN 32

typedef struct {
    union INST {
        uint32_t inst;
        uint8_t  readin[4];
    }inst;
    uint64_t addr;
    int len;

    INS32 *ptr_32;
    uint32_t imm;
    uint8_t rd, rs1, rs2;
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