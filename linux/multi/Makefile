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
#  Revision 1.3  1996/07/13 05:01:39  jussi
#  Updated to compile on Linux.
#
#  Revision 1.2  1996/07/08 18:15:00  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.1  1996/03/27 18:04:14  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.1  1995/10/15 19:12:19  jussi
#  Initial revision.

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
