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

ARCH_FLAGS = -DSGI -I/usr/include/X11 -I/usr/freeware/include
ARCH_LDFLAGS = -L/usr/lib/X11 -L/usr/freeware/lib -R /usr/lib/X11
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST = -lnsl

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
