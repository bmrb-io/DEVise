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
#  Revision 1.3  1996/11/23 20:57:18  jussi
#  Added ARCH_SYSLIBS.
#
#  Revision 1.2  1996/07/18 14:09:56  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.1  1996/07/17 00:27:38  jussi
#  Initial revision.

############################################################

ARCH_FLAGS   = -DULTRIX -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.pmax_ul43a
