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
#  Revision 1.2  1997/01/18 18:58:29  jussi
#  Removed inclusion of Xaw and related libraries.
#
#  Revision 1.1  1996/10/17 20:50:38  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl
# Note: Purify is not available on Pentium/Solaris.
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
