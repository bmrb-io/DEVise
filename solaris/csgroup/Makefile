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
#  Revision 1.2  1998/02/12 17:18:19  wenger
#  Merged through collab_br_2; updated version number to 1.5.1.
#
#  Revision 1.1.6.1  1998/02/12 05:13:13  taodb
#  Removed -g option
#
#  Revision 1.1  1997/09/26 02:30:42  taodb
#  *** empty log message ***
#
#  Revision 1.2  1997/01/18 18:58:29  jussi
#  Removed inclusion of Xaw and related libraries.
#
#  Revision 1.1  1996/10/17 20:50:38  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_FLAGS += -DDEBUG
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
