test: network.o smart.o inq.o
	g++ -g -L/s/tcl/lib -I/s/tcl/include -I/s/X11/include tipster.c \
        network.o smart.o inq.o  -ltk -ltcl -lX11 -lm -lsocket -lnsl \

inq.o: inq.c
	g++ -g -c -I/s/tcl/include -I/s/X11/include  inq.c

smart.o: smart.c
	g++ -g -c -I/s/tcl/include -I/s/X11/include smart.c

network.o: network.c
	g++ -g -c network.c

clean:
	rm -f *.o a.out

