#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1998
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

ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
#ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
#ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -ldl -lX11 -lm
ARCH_SYSLIBS = -ltk4.2 -ltcl7.6 -ldl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
