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
#  Revision 1.4  1996/11/23 20:56:20  jussi
#  Removed -lpthread.
#
#  Revision 1.3  1996/09/26 18:53:46  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.2  1996/05/20 18:38:29  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:10  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include 
ARCH_LDFLAGS =

VPATH = ../../graphics.new
include $(VPATH)/Makefile.base
include Makefile.depend
