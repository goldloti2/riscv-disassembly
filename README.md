# RISC-V Disassembler

This is a self-made RISC-V Disassembler, with RV64I, M, A, F, D extension.  
Still work in progress.  

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
