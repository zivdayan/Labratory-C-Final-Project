 GLOBAL_DEPS = glocal_consts.h

assembler: assembler.o preprocessor.o $(GLOBAL_DEPS)
	gcc -g -Wall -ansi -pedantic assembler.o preprocessor.o -o assembler

assembler.o: assembler.c preprocessor.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocessor.o: preprocessor.c preprocessor.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic preprocessor.c -o preprocessor.o

firstPass.o: firstPass.c firstPass.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic firstPass.c -o firstPass.o

secondPass.o: secondPass.c secondPass.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic secondPass.c -o secondPass.o

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic utils.c -o utils.o

frontend.o: frontend.c frontend.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic frontend.c -o frontend.o