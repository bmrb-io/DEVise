ARCH_FLAGS = -DSUN -I../../parser -g

VPATH = ../../parser
include $(VPATH)/Makefile.base

depend:
	makedepend -I../../parser $(VPATH)/*.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

codegen.o: /usr/include/stdio.h ../../parser/parse.h
main.o: ../../parser/parse.h /usr/include/stdio.h
parse.o: /usr/include/stdio.h ../../parser/parse.h ../../parser/tokens.h
parse.o: ../../parser/codegen.h
