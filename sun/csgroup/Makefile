#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1998
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  $Id$

#  $Log$

############################################################

include ../Makefile.config

#ARCH_FLAGS += -DDEBUG
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl7.4/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sun
