OTHERS=-DTCLTK_FRONT_END
CC = g++
CFLAGS = -g  $(INCLUDE) $(OTHERS)

INCLUDE= -I/s/X11/include -I/s/tcl/include
LIBPATH = -L/s/tcl/lib  -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
LIBS = $(LIBPATH)  -lX11 -lm -ltk -ltcl -lsocket 

HDRS= HashTable.h Window.h Control.h LMAPI.h LMError.h Layout.h
UTIL_HDRS = DList.h Exit.h 

SRC= HashTable.C Window.C  Control.C LMAPI.C Layout.C
UTIL_SRC = DList.C Exit.c 

OBJS= HashTable.o Window.o DList.o Exit.o Control.o LMAPI.o Layout.o

MAIN=LMMain.C
EXEC=LM

$(EXEC) : $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) $(MAIN) $(OBJS) -o $(EXEC) $(LIBS)

wintest : main1.C $(OBJS)
	$(CC) main1.C $(OBJS) -o wintest

hashtest :  main.C $(OBJS)
	$(CC) main.C $(OBJS) 

%.o : %.C %.h
	$(CC) -c $(CFLAGS) $< 


clean : 
	\rm *.o $(EXEC)



















