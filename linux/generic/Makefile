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
#  Revision 1.1  1996/03/27 18:04:01  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.3  1996/02/26 16:44:32  jussi
#  Makes a statically linked executable.
#
#  Revision 1.2  1996/01/17 22:37:29  jussi
#  Removed -static from ARCH_LDPOST.
#
#  Revision 1.1  1995/10/15 19:12:16  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -lSM -lICE -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
