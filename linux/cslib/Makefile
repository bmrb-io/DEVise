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

############################################################

#  $Id$

#  $Log$

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
