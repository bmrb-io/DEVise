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
#  Revision 1.6  1996/01/09 16:44:08  jussi
#  Added -lnsl for network support.
#
#  Revision 1.5  1995/09/28 15:12:00  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS   = -DPENTIUM -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
