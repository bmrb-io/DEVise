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
#  Revision 1.8  1996/05/20 18:37:57  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.7  1996/02/13 16:44:17  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.6  1996/01/09 16:44:08  jussi
#  Added -lnsl for network support.
#
#  Revision 1.5  1995/09/28 15:12:00  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
