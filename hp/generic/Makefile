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

ARCH_FLAGS   = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lXaw -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -lSM -lICE -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
