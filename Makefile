assembler: assembler.o preassembler.o first_pass.o second_pass.o line_check.o utils.o
	gcc -ansi -Wall -pedantic assembler.o preassembler.o first_pass.o second_pass.o utils.o line_check.o -o assembler -lm

assembler.o: assembler.c
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

preassembler.o: preassembler.c
	gcc -c -ansi -Wall -pedantic preassembler.c -o preassembler.o

first_pass.o: first_pass.c
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o
	
second_pass.o: second_pass.c
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o
	
line_check.o: line_check.c
	gcc -c -ansi -Wall -pedantic line_check.c -o line_check.o

utils.o: utils.c
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o -lm
