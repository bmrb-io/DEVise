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
#  Revision 1.4  1996/11/23 20:55:53  jussi
#  Added ARCH_SYSLIBS and removed -lpthread.
#
#  Revision 1.3  1996/09/26 18:53:43  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.2  1996/05/20 18:38:24  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:07  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib 
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
