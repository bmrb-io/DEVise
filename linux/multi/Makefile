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
#  Revision 1.1  1996/03/27 18:04:14  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.1  1995/10/15 19:12:19  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
