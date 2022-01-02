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

run1:rv-dasm.o test1/test1.o
	./rv-dasm.o test1/test1.o

run2:rv-dasm.o test2/test1.o
	./rv-dasm.o test2/test1.o

gdb1:rv-dasm-d.o test1/test1.o
	gdb --args ./rv-dasm-d.o test1/test1.o

asm1:test1/test1.c riscv64/bin/riscv64-unknown-elf-gcc
	rm -f test1/test1.o
	./riscv64/bin/riscv64-unknown-elf-gcc -o test1/test1.o test1/test1.c
	ls test1 -l

asm2:test2/test1.c riscv64/bin/riscv64-unknown-elf-gcc
	rm -f test2/test1.o
	./riscv64/bin/riscv64-unknown-elf-gcc -c -o test2/test1.o test2/test1.c
	readelf -a test2/test1.o > test2/readelf.txt
	objdump -x test2/test1.o > test2/objdump.txt
	hexdump -C test2/test1.o > test2/hexdump.txt
	./riscv64/bin/riscv64-unknown-elf-objdump -d test2/test1.o > test2/dump-off.txt
	ls test2 -l

.PHONY: clean
clean:
	rm -f *.o
