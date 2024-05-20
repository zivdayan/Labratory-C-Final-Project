 GLOBAL_DEPS = global_consts.h
 COMPILE_DEPS = assembler.o frontend.o preprocessor.o firstPass.o secondPass.o utils.o backend.o


assembler: $(COMPILE_DEPS) $(GLOBAL_DEPS) 
	gcc -g -Wall -ansi -pedantic $(COMPILE_DEPS) $(GLOBAL_DEPS)  -o assembler

assembler.o: assembler.c preprocessor.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocessor.o: preprocessor.c preprocessor.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic preprocessor.c -o preprocessor.o

frontend.o: frontend.c frontend.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic frontend.c -o frontend.o

firstPass.o: firstPass.c firstPass.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic firstPass.c -o firstPass.o

secondPass.o: secondPass.c secondPass.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic secondPass.c -o secondPass.o

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic utils.c -o utils.o

backend.o: utils.c utils.h $(GLOBAL_DEPS)
	gcc -g -c -Wall -ansi -pedantic backend.c -o backend.o

clean:
	rm -rf *.o *.am *.ob *.ent *.ext assembler
