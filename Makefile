compile:src/main.c src/prt_hdr.c src/read_instr.c
	clear
	rm -f rv-dasm.o
	gcc -o rv-dasm.o src/main.c src/prt_hdr.c src/read_instr.c
	ls -l

debug:src/main.c src/prt_hdr.c src/read_instr.c
	clear
	rm -f rv-dasm-d.o
	gcc -g -o rv-dasm-d.o src/main.c src/prt_hdr.c src/read_instr.c
	ls -l

run:rv-dasm.o test1/test1.o
	./rv-dasm.o test1/test1.o

gdb:rv-dasm-d.o test1/test1.o
	gdb --args ./rv-dasm-d.o test1/test1.o

asm:test1/test1.c riscv64/bin/riscv64-unknown-elf-gcc
	rm -f test1/test1.o
	./riscv64/bin/riscv64-unknown-elf-gcc -o test1/test1.o test1/test1.c
	ls test1 -l

.PHONY: clean
clean:
	rm -f *.o
