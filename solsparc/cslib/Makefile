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
#  Revision 1.2  1997/01/18 20:48:46  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.1  1996/10/18 14:04:30  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
