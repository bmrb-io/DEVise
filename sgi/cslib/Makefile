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
#  Revision 1.4  1998/03/02 22:31:55  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.3  1998/02/23 23:23:21  wenger
#  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.
#
#  Revision 1.2.16.1  1998/02/23 19:52:44  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.2  1997/01/18 20:49:21  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.1  1996/10/17 20:50:37  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/local/lib -L /usr/freeware/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
