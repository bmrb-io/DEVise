ARCH_FLAGS = -DPENTIUM -I../../parser

VPATH = ../../parser
include $(VPATH)/Makefile.base

depend:
	makedepend -I../../parser $(VPATH)/*.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

../../parser/codegen.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
../../parser/codegen.o: ../../parser/parse.h
../../parser/main.o: ../../parser/parse.h /usr/include/stdio.h
../../parser/main.o: /usr/include/sys/feature_tests.h
../../parser/parse.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
../../parser/parse.o: ../../parser/parse.h ../../parser/tokens.h
../../parser/parse.o: ../../parser/codegen.h
