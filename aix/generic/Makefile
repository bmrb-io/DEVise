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
#  Revision 1.3  1996/05/31 15:45:25  jussi
#  A customized Makefile.base.aix is used in AIX.
#
#  Revision 1.2  1996/05/22 21:00:37  jussi
#  Merged Makefile.base.aix with Makefile.base.
#
#  Revision 1.1  1996/05/16 19:34:46  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DAIX -I/u2/ratnakar/tcl/include -I/u2/ratnakar/tk/include
ARCH_LDFLAGS = -L/u2/ratnakar/tcl/lib -L/u2/ratnakar/tk/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lXaw -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.aix
