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
#  Revision 1.4  1996/11/23 20:57:00  jussi
#  Added ARCH_SYSLIBS.
#
#  Revision 1.3  1996/05/31 15:43:44  jussi
#  A customized Makefile.base.aix is used in AIX.
#
#  Revision 1.2  1996/05/22 21:00:36  jussi
#  Merged Makefile.base.aix with Makefile.base.
#
#  Revision 1.1  1996/05/16 19:34:44  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DAIX -I/u2/ratnakar/tcl/include -I/u2/ratnakar/tk/include
ARCH_LDFLAGS = -L/u2/ratnakar/tcl/lib -L/u2/ratnakar/tk/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base.aix
include $(VPATH)/Makefile.aix
