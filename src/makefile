assembler: assembler.o preprocessor.o
	gcc -g -Wall -ansi -pedantic assembler.o preprocessor.o -o assembler

assembler.o: assembler.c preprocessor.h
	gcc -g -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocessor.o: preprocessor.c preprocessor.h
	gcc -g -c -Wall -ansi -pedantic preprocessor.c -o preprocessor.o