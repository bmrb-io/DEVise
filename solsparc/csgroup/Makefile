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
#  Revision 1.1  1998/02/13 15:02:14  wenger
#  Added csgroup directory and makefile.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
