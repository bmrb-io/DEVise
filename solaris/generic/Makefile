#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1995
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
#  Revision 1.5  1995/09/28 15:12:00  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS   = -DPENTIUM -I/usr/X11R6/include -I/s/tcl/include
ARCH_LDFLAGS = -L /usr/X11R6/lib -L /s/tcl/lib -R /usr/X11R6/lib
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
