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
#  Revision 1.11  1997/04/03 21:31:42  wenger
#  Eliminated unnecessary tcl dependencies in graphics directory.
#
#  Revision 1.10  1996/05/20 18:37:58  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.9  1996/04/20 19:57:23  kmurli
#  no changes done.
#
#  Revision 1.8  1996/04/05 20:15:41  wenger
#  Fixed error causing pure virtual function to be called
#  if a session was closed during a query; fixed an error
#  in one of the Linux Makefiles; updated other Makefiles
#  to allow testWindowRep to be built on all architectures.
#
#  Revision 1.7  1996/02/13 16:44:18  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.6  1995/12/02 21:51:49  jussi
#  Added -I/s/tcl/include.
#
#  Revision 1.5  1995/09/28 15:12:01  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = 
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../graphics
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
