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
#  Revision 1.13.18.2  1997/12/04 15:50:15  wenger
#  Undid Haihong's temporary changes.
#
#  Revision 1.13.18.1  1997/12/03 22:53:03  whh
#  Devise now uses new C++ expression evaluation code instead of Tcl
#  interpreter.
#
#  Revision 1.13  1996/11/23 20:54:40  jussi
#  Removed -lpthread.
#
#  Revision 1.12  1996/11/18 18:14:21  donjerko
#  Added final touches for DTE
#
#  Revision 1.11  1996/09/26 18:54:01  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.10  1996/05/20 18:37:59  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.9  1996/04/20 19:57:31  kmurli
#  no changes done.
#
#  Revision 1.8  1996/02/13 16:44:19  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.7  1995/09/28 15:03:14  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include 
ARCH_LDFLAGS = -lsocket -lnsl

VPATH = ../../graphics.new
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
