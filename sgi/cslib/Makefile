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
#  Revision 1.1  1996/10/17 20:50:37  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSGI -I/usr/include/X11 -I/usr/local/include
ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/local/lib
ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
