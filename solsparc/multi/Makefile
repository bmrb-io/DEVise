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
#  Revision 1.2  1996/05/20 18:38:30  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:12  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib 
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
