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
#  Revision 1.1  1996/10/17 20:50:36  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
