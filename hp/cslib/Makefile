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

ARCH_FLAGS = -DHPUX -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lXaw -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST =
ARCH_COLLECTOR	= \
	/s/gcc-2.7.1/hp700_ux90/lib/gcc-lib/hppa1.1-hp-hpux9.01/2.7.1/ld

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
