
#
# Solaris
#
ARCH_FLAGS = -DSolaris
DEBUG_FLAG = -gstabs
TCL_INCS = -I/s/tcl/include
TCL_LIBS = -L/s/tcl/lib -ltk -ltcl
X_INCS = -I/s/X11/include
X_LIBS = -L/s/X11/lib -lX11
SOCKET_LIBS = -lsocket -lnsl

INCS = $(TCL_INCS) $(X_INCS) -I. -Iinclude

CC = gcc
CCC = g++

LDFLAGS = -L. -lm $(TCL_LIBS) $(X_LIBS) $(SOCKET_LIBS)

DEFINES = $(DEBUG_FLAG) $(ARCH_FLAGS)

CFLAGS = -Wall -Wno-unused $(INCS) $(DEFINES)

.c.o: 
	$(CCC) -c $(CFLAGS) $<

.C.o: 
	$(CCC) -c $(CFLAGS) $<

.o:
	$(CCC) -o $@ $@.o $(LDFLAGS)

default: EmbeddedTk
all: EmbeddedTk

ETK_OBJS = main.o EmbeddedTk.o TkWindowRep.o
EmbeddedTk: $(ETK_OBJS)
	$(CCC) -o EmbeddedTk $(ETK_OBJS) $(LDFLAGS)

.PHONY: clean mostlyclean

clean: mostlyclean
	-rm client server

mostlyclean:
	-rm $(OBJ)
	-rm *.o

depend:
	$(CCC) -M $(CFLAGS) *.[cC] > Makefile.depend

include Makefile.depend

