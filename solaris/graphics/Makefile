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
#  Revision 1.7  1996/02/13 16:44:18  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.6  1995/12/02 21:51:49  jussi
#  Added -I/s/tcl/include.
#
#  Revision 1.5  1995/09/28 15:12:01  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS = -DPENTIUM -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../graphics
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
