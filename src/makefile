assembler: assembler.o
	gcc -g -Wall -ansi -pedantic assembler.o -o assembler

assembler.o: assembler.c preprocessor.o
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocessor.o: preprocessor.c preprocessor.h
	gcc -c -Wall -ansi -pedantic preprocessor.c -o preprocessor.o