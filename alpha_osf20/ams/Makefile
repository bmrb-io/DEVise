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
#  Revision 1.2  1996/07/18 14:31:52  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.1  1996/07/14 20:34:37  jussi
#  Initial revision.

############################################################

ARCH_FLAGS   = -DOSF -I/s/gcc/lib/g++-include -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -L/s/gcc/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.alpha_osf20
