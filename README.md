# RISC-V Disassembler

This is a self-made RISC-V Disassembler, with RV64I, M, A, F, D, and RVC (compressed) extension.  
Still work in progress.  

## Future Plan

- read the symbol table and others

## Note

2021/12/17
- Init update
- Bug: segmentation fault after printing out several instruction lines
    - at instrution #69: 10172: 23 e0 00 ef JAL
    - segmentation fault after print_inst() returned
    - next line is: 10176: 7501b503 ld

2021/12/20
- Bug fixed:
    - segmentation fault @2021/12/17
        - cause: did not handling unknown instructions (which ptr_32 is NULL)
    - invalid format in instr32.csv
- Read registers and immediate in 32-bit instructions
    - now can read registers and immediate, and print them out
    - some instructions are implemented but not tested yet (mainly A, F, D extention sets)
    - maybe can print in different format depend on the instructions

2021/12/22
- Bug fixed:
    - segmentation fault in parse_instr(), read_instr.c
        - cause: cannot find matched instruction, letting inst->ptr_32 be a NULL pointer
- Slightly adjust the final output format

2021/12/28
- New features: RVC instruction set (2/3 tables finished)
    - rewrite existing functions to process RVC (print_inst(), parse_instr(), and read_ins_file())
    - instr16.csv for RVC instruction set

2022/1/2
- RVC instruction set (cont.)
    - all instructions are in instr16.csv
    - outputs and immediate parsing of instructions in 3rd table have not finish yet (outputs not implemented yet, immediate finished but not tested)
- Add another test code in test2/
    - same source code, but without linking

2022/1/3
- RVC instruction set finish
    - not all instructions are tested