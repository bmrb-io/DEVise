
CC = gcc
CFLAGS = -g
.c.o: 
	$(CC) -c $(CFLAGS) $*.c

ALLSRC= main.c parse.c codegen.c


parser: main.o parse.o lex.yy.o codegen.o
	$(CC) -o parser main.o parse.o codegen.o lex.yy.o -ll

lex.yy.o: scan.lex tokens.h
	lex scan.lex
	$(CC) -c lex.yy.c
	
depend:
	makedepend $(ALLSRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: parse.h /usr/include/stdio.h /usr/include/ansi_compat.h
parse.o: /usr/include/stdio.h /usr/include/ansi_compat.h parse.h tokens.h
parse.o: codegen.h
codegen.o: /usr/include/stdio.h /usr/include/ansi_compat.h parse.h
