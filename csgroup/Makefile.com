#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  Makefile for the client-server library.

############################################################

#  $Id$

#  $Log$
#  Revision 1.2.2.1  1998/01/28 22:42:15  taodb
#  Added support for group communicatoin
#
#  Revision 1.1  1996/10/18 15:19:52  jussi
#  Initial revision.
#

############################################################

LD = ld
CC = g++
C  = gcc
AR = ar
RANLIB = ranlib

.c.o:
	$(CC) -c $(CFLAGS) $<
.C.o:
	$(CC) -c $(CFLAGS) $<

CFLAGS = $(ARCH_FLAGS) -Iinclude -Wall -Wno-unused -O \
        -DGRAPHICS -DLIBCS -DUNIX -DRAWMOUSEEVENTS

LIB = libcs.a

TESTSRC = client.C server.C
TESTOBJ = $(TESTSRC:.C=.o)

all: client server

client: client.o $(LIB)
	$(CC) -o $@ $(ARCH_LDFLAGS) $@.o -L. -lcs \
        $(ARCH_SYSLIBS) $(ARCH_LDPOST)

server: server.o $(LIB)
	$(CC) -o $@ $(ARCH_LDFLAGS) $@.o -L. -lcs \
        $(ARCH_SYSLIBS) $(ARCH_LDPOST)

.PHONY: clean mostlyclean

clean: mostlyclean
	-rm -f client server

mostlyclean:
	-rm -f $(TESTOBJ)
