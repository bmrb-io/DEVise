#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1998
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
#  Revision 1.2  1998/03/02 22:31:48  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.1  1998/02/26 18:55:20  wenger
#  Got everything to compile on haha -- still have a link problem, though.
#

############################################################

include ../Makefile.config

# For some reason Client.C doesn't compile if /usr/include is in
# the include path...
ARCH_FLAGS = -DSGI -I/usr/include/X11 -I/usr/freeware/include
ARCH_LDFLAGS = -L/usr/lib/X11 -L/usr/freeware/lib -R /usr/lib/X11
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST = -lnsl

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
