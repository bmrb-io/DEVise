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
#  Revision 1.5.16.1  1998/02/23 19:52:22  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.5  1997/01/18 20:49:20  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/11/23 20:57:38  jussi
#  Added ARCH_SYSLIBS.
#
#  Revision 1.3  1995/12/29 18:54:35  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:33  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:11:55  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSGI -I/usr/include/X11 -I/usr/local/include \
	-I/usr/freeware/include
ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/local/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
